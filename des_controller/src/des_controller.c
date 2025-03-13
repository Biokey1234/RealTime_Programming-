#include <stdio.h>
#include <stdlib.h>

#include <errno.h>
#include <unistd.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <process.h>
#include <string.h>

#include <sys/neutrino.h>
#include <sys/netmgr.h>
#include "des.h"

void *start_state(); /* Start*/
void *left_lock();/* LEFT LOCK*/
void *right_lock();/* RIGHT LOCK*/
void *left_unlock();/* LEFT UNLOCK*/
void *right_unlock();/* RIGHT UNLOCK*/
void *door_opened();/* DOOR OPENED*/
void *weigh_scale();/* WEIGH SCALE*/
void *left_close();/* LEFT CLOSE*/
void *right_close();/* RIGHT CLOSE*/
typedef void *(*StateFunc)();
Display display;
int isWeighed;
/*
 * Create channel for inputs process to attach and attach to display's channel
 * Receive person object from inputs, send EOK back to inputs
 *  Detach display's channel and close connection to inputs
 */
int main(int argc, char* argv[]) {

	int inputChid;
	int displayChid;
	int rcvid;
	pid_t spid;
	spid = atoi(argv[1]);
	Person person;
	StateFunc stateptr = start_state;
	if (argc != 2) {
		fprintf(stderr, "Missing PID to display\n");
		exit(EXIT_FAILURE);
	}

	inputChid = ChannelCreate(0);

	if (inputChid == -1) {
		fprintf(stderr, "ERROR: Failed to create channel in des_controller\n");
		exit(EXIT_FAILURE);
	}

	displayChid = ConnectAttach(ND_LOCAL_NODE, spid, 1, _NTO_SIDE_CHANNEL, 0);
	if (displayChid == -1) {
		fprintf(stderr, "ERROR: Failed to connect to display\n");
		exit(EXIT_FAILURE);
	}
	printf("The controller is running as PID: %d\n", getpid());
	printf("Waiting for Person...\n");
	while (1) {
		rcvid = MsgReceive(inputChid, &person, sizeof(person), NULL);
		if (strcmp(person.msg, inMessage[EXIT]) == 0) {
			printf("Exiting controller.\n");
			strcpy(display.msg, "exit");
			MsgSend(displayChid, &display, sizeof(display), NULL, 0);
			break;
		}
		stateptr = (StateFunc) (*stateptr)(&person);
		MsgSend(displayChid, &display, sizeof(display), NULL, 0);
		MsgReply(rcvid, EOK, NULL, 0);
	}

	ConnectDetach(displayChid);
	ChannelDestroy(inputChid);

	return EXIT_SUCCESS;
}

/*
 * States:
 * start state, left lock state, right lock state, left unlock state, right unlock state
 * Door Opened State, Weigh state, Left Close State, Right Close State
 *
 */

void *start_state(Person *per) {
	if (strcmp(per->msg, inMessage[LS]) == 0) {
		strcpy(display.msg, outMessage[LEFT_SCAN]);
		strcat(display.msg, per->id);
		return left_lock;
	}
	if (strcmp(per->msg, inMessage[RS]) == 0) {
		strcpy(display.msg, outMessage[RIGHT_SCAN]);
		strcat(display.msg, per->id);
		return right_lock;
	} else {
		strcpy(display.msg, errorMessage[SCAN]);
		return start_state;
	}
}

void *left_lock(Person *per) {
	if (per->direction == OUTBOUND && (strcmp(per->msg, inMessage[GLU]) == 0)
			&& (isWeighed)) {
		strcpy(display.msg, outMessage[GUARD_LEFT_UNLOCK]);
		return left_unlock;
	}
	if (per->direction == INBOUND && (strcmp(per->msg, inMessage[GLU]) == 0)
			&& (!isWeighed)) {
		strcpy(display.msg, outMessage[GUARD_LEFT_UNLOCK]);
		return left_unlock;
	} else {
		strcpy(display.msg, errorMessage[UNLOCK]);
		return left_lock;
	}
}

void *right_lock(Person *per) {
	if (per->direction == INBOUND && (strcmp(per->msg, inMessage[GRU]) == 0)
			&& (isWeighed)) {
		strcpy(display.msg, outMessage[GUARD_RIGHT_UNLOCK]);
		return right_unlock;
	} else if (per->direction == OUTBOUND
			&& (strcmp(per->msg, inMessage[GRU]) == 0) && (!isWeighed)) {
		strcpy(display.msg, outMessage[GUARD_RIGHT_UNLOCK]);
		return right_unlock;
	} else {

		strcpy(display.msg, errorMessage[UNLOCK]);
		return right_lock;
	}
}

void *left_unlock(Person *per) {
	if (per->direction == OUTBOUND && (strcmp(per->msg, inMessage[LO]) == 0)
			&& (isWeighed)) {
		strcpy(display.msg, outMessage[LEFT_OPEN]);
		return door_opened;
	}
	if (per->direction == INBOUND && (strcmp(per->msg, inMessage[LO]) == 0)
			&& (!isWeighed)) {
		strcpy(display.msg, outMessage[LEFT_OPEN]);
		return door_opened;
	} else {
		strcpy(display.msg, errorMessage[OPEN]);
		return left_unlock;
	}
}

void *right_unlock(Person* per) {
	if (per->direction == INBOUND && (strcmp(per->msg, inMessage[RO]) == 0)
			&& (isWeighed)) {
		strcpy(display.msg, outMessage[RIGHT_OPEN]);
		return door_opened;
	} else if (per->direction == OUTBOUND
			&& (strcmp(per->msg, inMessage[RO]) == 0) && (!isWeighed)) {
		strcpy(display.msg, outMessage[RIGHT_OPEN]);
		return door_opened;
	} else {
		strcpy(display.msg, errorMessage[OPEN]);
		return right_unlock;
	}
}

void *door_opened(Person *per) {
	if ((strcmp(per->msg, inMessage[WS]) == 0) && (!isWeighed)) {
		isWeighed = 1;
		strcpy(display.msg, outMessage[WEIGHT_SCALE]);
		char weight[100];
		itoa(per->weight, weight, 10);
		strcat(display.msg, weight);
		return weigh_scale;
	} else if (per->direction == INBOUND
			&& (strcmp(per->msg, inMessage[RC]) == 0) && (isWeighed)) {
		strcpy(display.msg, outMessage[RIGHT_CLOSED]);
		return right_close;
	} else if (per->direction == OUTBOUND
			&& (strcmp(per->msg, inMessage[LC]) == 0) && (isWeighed)) {
		strcpy(display.msg, outMessage[LEFT_CLOSED]);
		return left_close;
		//
	} else {
		strcpy(display.msg,
				"ERROR: Must do weight scaling OR closing door next");
		return door_opened;
	}
}

void *weigh_scale(Person *per) {
	if (per->direction == INBOUND && (strcmp(per->msg, inMessage[LC]) == 0)) {
		strcpy(display.msg, outMessage[LEFT_CLOSED]);
		return left_close;
	} else if (per->direction == OUTBOUND
			&& (strcmp(per->msg, inMessage[RC]) == 0)) {
		strcpy(display.msg, outMessage[RIGHT_CLOSED]);
		return right_close;
	} else {
		strcpy(display.msg, errorMessage[CLOSE]);
		return weigh_scale;
	}
}

void *left_close(Person *per) {
	if (per->direction == INBOUND && (strcmp(per->msg, inMessage[GLL]) == 0)) {
		strcpy(display.msg, outMessage[GUARD_LEFT_LOCK]);
		return right_lock;
	} else if (per->direction == OUTBOUND
			&& (strcmp(per->msg, inMessage[GLL]) == 0)) {
		strcpy(display.msg, outMessage[GUARD_LEFT_LOCK]);
		strcat(display.msg, "\nWaiting for Person...");
		isWeighed = 0;
		return start_state;
	} else {
		strcpy(display.msg, errorMessage[LOCK]);
		return left_close;
	}
}

void *right_close(Person *per) {
	if (per->direction == OUTBOUND && (strcmp(per->msg, inMessage[GRL]) == 0)) {
		strcpy(display.msg, outMessage[GUARD_RIGHT_LOCK]);
		return left_lock;
	} else if (per->direction == INBOUND
			&& (strcmp(per->msg, inMessage[GRL]) == 0)) {
		strcpy(display.msg, outMessage[GUARD_RIGHT_LOCK]);
		strcat(display.msg, "\nWaiting for Person...");
		isWeighed = 0;
		return start_state;
	} else {
		strcpy(display.msg, errorMessage[LOCK]);
		return right_close;
	}
}
