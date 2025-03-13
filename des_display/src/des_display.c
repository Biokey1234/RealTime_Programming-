
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <process.h>
#include <unistd.h>

#include <sys/neutrino.h>
#include "../../des_controller/src/des.h"
/*
 * Server to Controller
 * Open connection to client
 * Receive message from Controller
 * Close connection to client controller
 */

int main(void) {
	int chid;
	int rcvid;
	Display display;
	chid = ChannelCreate(0);
	if (chid == -1) {
		fprintf(stderr, "ERROR: Failed to create channel in des_display\n");
		perror(NULL);
		exit(EXIT_FAILURE);
	}
	printf("The display is running as PID %d\n", getpid());

	while (1) {
		rcvid = MsgReceive(chid, &display, sizeof(display), NULL);

		if (rcvid == -1) {
			printf("\nERROR: Failed to receive message from Controller \n");
			exit(EXIT_FAILURE);
		}

		if (strcmp(display.msg, "exit") == 0) {
			exit(EXIT_FAILURE);
		}
		printf("%s\n", display.msg); //display message
		MsgReply(rcvid, EOK, NULL, 0);
	}

	ChannelDestroy(chid);
	return EXIT_SUCCESS;
}
