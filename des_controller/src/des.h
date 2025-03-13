/*
 * des.h
 *
 *  Created on: Mar 13, 2025
 *      Author: keymu
 */

#ifndef DOOR_ENTRY_SYSTEM_H_
#define DOOR_ENTRY_SYSTEM_H_

#define NUM_STATES 9
typedef enum {
	START = 0,
	LEFT_LOCK = 1,
	RIGHT_LOCK = 2,
	LEFT_UNLOCK = 3,
	RIGHT_UNLOCK = 4,
	DOOR_OPENED = 5,
	WEIGH = 6,
	LEFT_CLOSE_STATE = 7,
	RIGHT_CLOSE_STATE = 8
} State;

#define NUM_INPUTS 12
typedef enum {
	LS = 0,
	RS = 1,
	WS = 2,
	LO = 3,
	RO = 4,
	LC = 5,
	RC = 6,
	GRU = 7,
	GRL = 8,
	GLL = 9,
	GLU = 10,
	EXIT = 11
} Inputs;

const char *inMessage[NUM_INPUTS] = { "ls", "rs", "ws", "lo", "ro", "lc", "rc",
		"gru", "grl", "gll", "glu", "exit" };

#define NUM_OUTPUTS 11
typedef enum {
	LEFT_SCAN = 0,
	RIGHT_SCAN = 1,
	WEIGHT_SCALE = 2,
	LEFT_OPEN = 3,
	RIGHT_OPEN = 4,
	LEFT_CLOSED = 5,
	RIGHT_CLOSED = 6,
	GUARD_RIGHT_UNLOCK = 7,
	GUARD_RIGHT_LOCK = 8,
	GUARD_LEFT_LOCK = 9,
	GUARD_LEFT_UNLOCK = 10
} Output;

const char *outMessage[NUM_OUTPUTS] = { "Person left scanned ID, ID = ",
		"Person right scanned ID, ID = ", "Person weighed, Weight = ",
		"Person opened left door", "Person opened right door",
		"Left door closed (automatically)", "Right door closed (automatically)",
		"Right door unlocked by guard", "Right door locked by guard",
		"Left door locked by guard", "Left door unlocked by guard" };

#define NUM_ERROR 5
typedef enum {
	SCAN = 0, OPEN = 1, CLOSE = 2, UNLOCK = 3, LOCK = 4
} Error;

const char *errorMessage[NUM_ERROR] = { "ERROR: Must scan next",
		"ERROR: Must open next", "ERROR: Must close", "ERROR: Must unlock next",
		"ERROR: Must lock next" };

typedef struct {
	char msg[100];
} Display;

#define INBOUND 1
#define OUTBOUND 0
typedef struct {
	char id[14];
	int weight;
	char msg[100];
	int direction;
} Person;

#endif /* DOOR_ENTRY_SYSTEM_H_ */
