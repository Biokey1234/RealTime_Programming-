#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <process.h>

#include <sys/neutrino.h>
#include <sys/netmgr.h>

#include "../../des_controller/src/des.h"
/*
 * Client for controller
 * Open a connection to the server (Controller)
 * Inputs client sends message to Controller server
 * Close connection to Controller's channel
 */

int main(int argc, char * argv[]) {
	int spid = 0;
	int coid;
	Person person;
	char idPersonal[27];
	char inMes[16];
	int pWeight;
	if (argc != 2) {
		fprintf(stderr, "Missing PID to controller\n");
		exit(EXIT_FAILURE);
	}
	spid = atoi(argv[1]);
	coid = ConnectAttach(ND_LOCAL_NODE, spid, 1, _NTO_SIDE_CHANNEL, 0);

	if (coid == -1) {
		fprintf(stderr, "Failed to connect to controller\n");
		exit(EXIT_FAILURE);
	}

	while (1) {
		printf(
				"Enter the event type (ls= left scan, rs= right scan, ws= weight scale, lo =left open, ro=right open, lc = left closed, rc = right closed , gru = guard right unlock, grl = guard right lock, gll=guard left lock, glu = guard left unlock, exit = exit programs)\n");
		scanf(" %s", inMes);

		if (strcmp(inMes, inMessage[LS]) == 0) {
			printf("Enter the Person's ID:\n");
			scanf(" %s", idPersonal);
			strcpy(person.id, idPersonal);
			person.direction = INBOUND;
			strcpy(person.msg, inMessage[LS]);

		} else if (strcmp(inMes, inMessage[RS]) == 0) {
			printf("Enter the Person's ID:\n");
			scanf(" %s", idPersonal);
			strcpy(person.id, idPersonal);
			person.direction = OUTBOUND;
			strcpy(person.msg, inMessage[RS]);

		} else if (strcmp(inMes, inMessage[GLU]) == 0) {
			strcpy(person.msg, inMessage[GLU]);
		}

		else if (strcmp(inMes, inMessage[GRU]) == 0) {
			strcpy(person.msg, inMessage[GRU]);
		}

		else if (strcmp(inMes, inMessage[RO]) == 0) {
			strcpy(person.msg, inMessage[RO]);
		}

		else if (strcmp(inMes, inMessage[LO]) == 0) {
			strcpy(person.msg, inMessage[LO]);
		}

		else if (strcmp(inMes, inMessage[LC]) == 0) {
			strcpy(person.msg, inMessage[LC]);
		}

		else if (strcmp(inMes, inMessage[RC]) == 0) {
			strcpy(person.msg, inMessage[RC]);
		}

		else if (strcmp(inMes, inMessage[GLL]) == 0) {
			strcpy(person.msg, inMessage[GLL]);
		}

		else if (strcmp(inMes, inMessage[GRL]) == 0) {
			strcpy(person.msg, inMessage[GRL]);
		}

		else if (strcmp(inMes, inMessage[GLU]) == 0) {
			strcpy(person.msg, inMessage[GLU]);
		}

		else if (strcmp(inMes, inMessage[GRU]) == 0) {
			strcpy(person.msg, inMessage[GRU]);
		}

		else if (strcmp(inMes, inMessage[WS]) == 0) {
			printf("Enter the Person's weight:\n");
			scanf("%d", &pWeight);
			person.weight = pWeight;
			strcpy(person.msg, inMessage[WS]);
		}

		else if (strcmp(inMes, inMessage[EXIT]) == 0) {
			strcpy(person.msg, inMessage[EXIT]);
			printf("Exiting Display.\n");
			MsgSend(coid, &person, sizeof(person), NULL, 0);
			break;
		} else {

			fflush(stdin);
		}
		MsgSend(coid, &person, sizeof(person), NULL, 0);

	}

	ConnectDetach(coid);
	return EXIT_SUCCESS;
}
