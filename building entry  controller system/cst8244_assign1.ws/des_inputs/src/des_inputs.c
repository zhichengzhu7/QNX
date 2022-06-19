/*
 * Filename:    des_controller.c
 * Date:        November 11, 2021
 * Course:      CST8244 Real-Time Programming
 * Author(s):   Michael Turner and Zhicheng Zhu
 * Function(s): int main(int argc, char* args[])
 *
 * Reads input from the command line, and sends the appropriate
 * message to des_controller.
 */

#include <stdio.h>
#include <stdlib.h>
#include <process.h>
#include <string.h>
#include <errno.h>
#include <sys/neutrino.h>
#include <sys/iofunc.h>
#include <sys/dispatch.h>
#include <sys/netmgr.h>
#include "../../des_controller/src/des.h"

send_t msg_send;
response_t msg_receive;

int coid;

int main(int argc, char* args[]) {
    char input[10];
    int person_id;
    float weight;
    pid_t controller_pid;

    if (argc != 2) {
        printf("Missing PID to controller");
        return EXIT_FAILURE;
	}

    controller_pid = atoi(args[1]);
    if ((coid = ConnectAttach(ND_LOCAL_NODE, controller_pid, 1, _NTO_SIDE_CHANNEL, 0)) == -1) {
		printf("ERROR: Could not connect to controller\n");
		exit(EXIT_FAILURE);
	}

	printf("Enter the event type (ls = left scan, rs = right scan, ws = weight scale, lo = left open, ro = right open, lc = left closed, "
			"rc = right closed, gru = guard right unlock, grl = guard right lock, gll = guard left lock, glu = guard left unlock)\n");

    while (1) {
        scanf(" %s", input);

        if (strcmp(input, "ls") == 0 || strcmp(input, "rs") == 0) {
            printf("Enter your ID: \n");
            scanf("%d", &person_id);
            msg_send.person_id = person_id;
            if (strcmp(input, "ls") == 0)
                msg_send.input = LEFT_SCAN_I;
            else
                msg_send.input = RIGHT_SCAN_I;
        }
        if (strcmp(input, "ws") == 0) {
            printf("Enter your weight:\n");
            scanf("%f", &weight);
            msg_send.weight = weight;
            msg_send.input = WEIGHT_SCAN_I;
        }
        if (strcmp(input, "lo") == 0) msg_send.input = LEFT_OPEN_I;
        if (strcmp(input, "ro") == 0) msg_send.input = RIGHT_OPEN_I;
        if (strcmp(input, "lc") == 0) msg_send.input = LEFT_CLOSE_I;
        if (strcmp(input, "rc") == 0) msg_send.input = RIGHT_CLOSE_I;
        if (strcmp(input, "glu") == 0) msg_send.input = LEFT_UNLOCK_I;
        if (strcmp(input, "gll") == 0) msg_send.input = LEFT_LOCK_I;
        if (strcmp(input, "gru") == 0) msg_send.input = RIGHT_UNLOCK_I;
        if (strcmp(input, "grl") == 0) msg_send.input = RIGHT_LOCK_I;
        if (strcmp(input, "exit") == 0) {
            msg_send.input = EXIT_I;
            if (MsgSend(coid, &msg_send, sizeof(send_t), 0, 0) == -1) {
                printf("\nUnable to send message.\n");
                ConnectDetach(coid);
                return EXIT_FAILURE;
            }

            ConnectDetach(coid);
            return EXIT_SUCCESS;
        }

        if (MsgSend(coid, &msg_send, sizeof(msg_send) + 1, &msg_receive, sizeof(response_t)) == -1) {
            printf("MsgSend had an error.\n");
            return EXIT_FAILURE;
        }
    }

    ConnectDetach(coid);
    return EXIT_SUCCESS;
}
