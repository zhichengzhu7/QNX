/*
 * Filename:    des_controller.c
 * Date:        November 11, 2021
 * Course:      CST8244 Real-Time Programming
 * Author(s):   Michael Turner and Zhicheng Zhu
 * Function(s): int main(int argc, char* argv[])
 *
 * Reads input sent from des_inputs, sets the appropriate state (or prints an error message),
 * and then sends the message to the des_display containing the current state.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/neutrino.h>
#include <sys/netmgr.h>
#include <sys/iofunc.h>
#include <sys/dispatch.h>
#include "./des.h"

send_t msg_received; // The message received from the des_inputs
response_t msg_response; // The response to des_display

int main(int argc, char* argv[]) {
	State curr_state = READY_S; // The current state
	pid_t display_pid;
	int coid, chid, rcvid;
	int msg_send;
	int const flags = 0;

	if (argc != 2 ) {
			printf("Missing PID to controller");
			exit(EXIT_FAILURE);
	}

	display_pid = atoi(argv[1]);
	if ((chid = ChannelCreate(flags)) == -1) {
		fprintf(stderr, "ERROR: Could not create channel.\n");
		perror(NULL);
		exit(EXIT_FAILURE);
	}

	if ((coid = ConnectAttach(0, display_pid, 1, _NTO_SIDE_CHANNEL, flags)) == -1) {
		fprintf(stderr, "ERROR: Could not attach connection.\n");
		perror(NULL);
		exit(EXIT_FAILURE);
	}

	printf("Controller PID: %d\n", getpid());

	while (1) {
		msg_send = 0; // Keep track of whether there is a message being sent to des_display or not

		if ((rcvid = MsgReceive(chid, &msg_received, sizeof(send_t), NULL)) == -1) {
			printf("ERROR: Could not receive message from input.\n");
			return EXIT_FAILURE;
		}

		if (msg_received.input == EXIT_I) curr_state = EXIT_S;

		switch (curr_state) {
		case READY_S:
			if (msg_received.input == LEFT_SCAN_I) {
				curr_state = LEFT_SCAN_S;
				msg_send = 1;
			} else if (msg_received.input == RIGHT_SCAN_I) {
				curr_state = RIGHT_SCAN_S;
				msg_send = 1;
			}
			break;
		case LEFT_SCAN_S:
			if (msg_received.input == LEFT_UNLOCK_I) {
				curr_state = IN_LEFT_UNLOCK_S;
				msg_send = 1;
			}
			break;
		case RIGHT_SCAN_S:
			if (msg_received.input == RIGHT_UNLOCK_I) {
				curr_state = OUT_RIGHT_UNLOCK_S;
				msg_send = 1;
			}
			break;
		case IN_LEFT_UNLOCK_S:
			if (msg_received.input == LEFT_OPEN_I) {
				curr_state = IN_LEFT_OPEN_S;
				msg_send = 1;
			}
			break;
		case IN_LEFT_OPEN_S:
			if (msg_received.input == WEIGHT_SCAN_I) {
				curr_state = IN_WEIGHT_SCAN_S;
				msg_send = 1;
			}
			break;
		case IN_WEIGHT_SCAN_S:
			if (msg_received.input == LEFT_CLOSE_I) {
				curr_state = IN_LEFT_CLOSE_S;
				msg_send = 1;
			}
			break;
		case IN_LEFT_CLOSE_S:
			if (msg_received.input == LEFT_LOCK_I) {
				curr_state = IN_LEFT_LOCK_S;
				msg_send = 1;
			}
			break;
		case IN_LEFT_LOCK_S:
			if (msg_received.input == RIGHT_UNLOCK_I) {
				curr_state = IN_RIGHT_UNLOCK_S;
				msg_send = 1;
			}
			break;
		case IN_RIGHT_UNLOCK_S:
			if (msg_received.input == RIGHT_OPEN_I) {
				curr_state = IN_RIGHT_OPEN_S;
				msg_send = 1;
			}
			break;
		case IN_RIGHT_OPEN_S:
			if (msg_received.input == RIGHT_CLOSE_I) {
				curr_state = IN_RIGHT_CLOSE_S;
				msg_send = 1;
			}
			break;
		case IN_RIGHT_CLOSE_S:
			if (msg_received.input == RIGHT_LOCK_I) {
				curr_state = READY_S;
				msg_send = 1;
			}
			break;
		case OUT_RIGHT_UNLOCK_S:
			if (msg_received.input == RIGHT_OPEN_I) {
				curr_state = OUT_RIGHT_OPEN_S;
				msg_send = 1;
			}
			break;
		case OUT_RIGHT_OPEN_S:
			if (msg_received.input == WEIGHT_SCAN_I) {
				curr_state = OUT_WEIGHT_SCAN_S;
				msg_send = 1;
			}
			break;
		case OUT_WEIGHT_SCAN_S:
			if (msg_received.input == RIGHT_CLOSE_I) {
				curr_state = OUT_RIGHT_CLOSE_S;
				msg_send = 1;
			}
			break;
		case OUT_RIGHT_CLOSE_S:
			if (msg_received.input == RIGHT_LOCK_I) {
				curr_state = OUT_RIGHT_LOCK_S;
				msg_send = 1;
			}
			break;
		case OUT_RIGHT_LOCK_S:
			if (msg_received.input == LEFT_UNLOCK_I) {
				curr_state = OUT_LEFT_UNLOCK_S;
				msg_send = 1;
			}
			break;
		case OUT_LEFT_UNLOCK_S:
			if (msg_received.input == LEFT_OPEN_I) {
				curr_state = OUT_LEFT_OPEN_S;
				msg_send = 1;
			}
			break;
		case OUT_LEFT_OPEN_S:
			if (msg_received.input == LEFT_CLOSE_I) {
				curr_state = OUT_LEFT_CLOSE_S;
				msg_send = 1;
			}
			break;
		case OUT_LEFT_CLOSE_S:
			if (msg_received.input == LEFT_LOCK_I) {
				curr_state = READY_S;
				msg_send = 1;
			}
			break;
		case EXIT_S:
			MsgSend(coid, &msg_received, sizeof(send_t), &msg_response, sizeof(response_t));
			MsgReply(rcvid, EOK, &msg_response, sizeof(response_t));
			return EXIT_SUCCESS;
		}

		// If a message is to be sent to des_display, proceed
		if (msg_send == 1) {
			if (MsgSend(coid, &msg_received, sizeof(send_t), &msg_response, sizeof(response_t)) == -1) {
				fprintf(stderr, "ERROR: Could not send message.\n");
				perror(NULL);
				exit(EXIT_FAILURE);
			}
		}

		// Reply to des_inputs for another input event
		if (MsgReply(rcvid, EOK, &msg_response, sizeof(response_t)) == -1) {
			fprintf(stderr, "ERROR: Could not send reply for next message.\n");
			perror(NULL);
			exit(EXIT_FAILURE);
		}
	}

	// Disconnect from the channel
	ConnectDetach(coid);

	// Destroy the channel
	ChannelDestroy(chid);
	return EXIT_SUCCESS;
}
