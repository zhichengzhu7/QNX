/******************
 * Filename: des.h
 * Date: November 11, 2020
 * Course: CST8244 Real-Time Programming
 * Author: Michael Turner and Zhicheng Zhu
 *
 * Holds the lists of state transitions, states, messages, the response struct, and the send struct.
 *****************/


#ifndef DES_MVA_H_
#define DES_MVA_H_

#define NAME_ATTACH_DISPLAY "des_display"
#define NAME_ATTACH_CONTROLLER "des_controller"

// State transitions
#define NUM_STATES 20
typedef enum {
	READY_S = 0,
	LEFT_SCAN_S = 1,
	RIGHT_SCAN_S = 2,
	IN_LEFT_UNLOCK_S = 3,
	IN_RIGHT_UNLOCK_S = 4,
	IN_LEFT_OPEN_S = 5,
	IN_RIGHT_OPEN_S = 6,
	IN_WEIGHT_SCAN_S = 7,
	IN_LEFT_CLOSE_S = 8,
	IN_LEFT_LOCK_S = 9,
	IN_RIGHT_CLOSE_S = 10,
	OUT_RIGHT_UNLOCK_S = 11,
	OUT_RIGHT_OPEN_S = 12,
	OUT_WEIGHT_SCAN_S = 13,
	OUT_RIGHT_CLOSE_S = 14,
	OUT_RIGHT_LOCK_S = 15,
	OUT_LEFT_UNLOCK_S = 16,
	OUT_LEFT_OPEN_S = 17,
	OUT_LEFT_CLOSE_S = 18,
	EXIT_S = 19
} State;

// States
#define NUM_INPUTS 12
typedef enum {
	LEFT_SCAN_I = 0,
	RIGHT_SCAN_I = 1,
	LEFT_UNLOCK_I = 2,
	LEFT_OPEN_I = 3,
	LEFT_CLOSE_I = 4,
	LEFT_LOCK_I = 5,
	WEIGHT_SCAN_I = 6,
	RIGHT_UNLOCK_I = 7,
	RIGHT_OPEN_I = 8,
	RIGHT_CLOSE_I = 9,
	RIGHT_LOCK_I = 10,
	EXIT_I = 11
} Input_t;

// Responses
#define NUM_OUTPUTS 12
typedef enum {
	READY_O = 0,
	SCAN_O = 1,
	WEIGHT_SCAN_O= 2,
	LEFT_OPEN_O = 3,
	RIGHT_OPEN_O = 4,
	LEFT_CLOSE_O = 5,
	RIGHT_CLOSE_O = 6,
	LEFT_UNLOCK_O = 7,
	RIGHT_UNLOCK_O = 8,
	RIGHT_LOCK_O = 9,
	LEFT_LOCK_O = 10,
	EXIT_O = 11
} Output_t;

// Messages sent to display
const char *outMessage[NUM_OUTPUTS] = {
	"Controller PID: ",
	"Person ID: ",
	"Person weight: ",
	"Opened left door.",
	"Opened right door.",
	"Closed left door.",
	"Closed right door.",
	"Guard unlocked left door. ",
	"Guard unlocked right door. ",
	"Guard locked right door. ",
	"Guard locked left door. ",
	"Exiting..."
};

// Response data
struct response {
	int statusCode;
	char errorMsg[128];
} typedef response_t;

// Input data
struct send {
	Input_t input;
	float weight;
	int person_id;
} typedef send_t;

#endif
