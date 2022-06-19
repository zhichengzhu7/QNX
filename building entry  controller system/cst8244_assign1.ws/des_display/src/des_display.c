/*
 * Filename:    des_controller.c
 * Date:        November 11, 2021
 * Course:      CST8244 Real-Time Programming
 * Author(s):   Michael Turner and Zhicheng Zhu
 * Function(s): int main(void)
 *
 * Reads message sent from des_controller and prints the appropriate
 * message to the console.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/neutrino.h>
#include <sys/netmgr.h>
#include <errno.h>
#include <sys/iofunc.h>
#include <sys/dispatch.h>
#include "../../des_controller/src/des.h"

send_t msg_received;
response_t response;

int main(void) {
    int chid;
    int rcvid;
    if ((chid = ChannelCreate(0)) == -1){
    	printf("ERROR: Could not create Channel");
		exit(EXIT_FAILURE);
    }

    printf("Display's PID: %d\n", getpid());

    while (1) {
        //Call MsgReceive() to receive Display object from controller
        if ((rcvid = MsgReceive(chid, &msg_received, sizeof(send_t), NULL)) == -1) {
            printf("Error receiving message from controller.");
            return EXIT_FAILURE;
        }

        //Call MsgReply(), sending EOK back to the controller
        if (MsgReply(rcvid, EOK, &response, sizeof(response_t)) == -1) {
            printf("Unable to reply to message.");
            return EXIT_FAILURE;
        }

        //IF message == ID_SCAN THEN
        if (msg_received.input == LEFT_SCAN_I || msg_received.input == RIGHT_SCAN_I)
        	printf("%s %d\n", outMessage[SCAN_O], msg_received.person_id);
        //ELSE IF message = WEIGHED THEN
        //Print person has been weighed and their weight
        else if (msg_received.input == WEIGHT_SCAN_I)
        	printf("Person ID: %d has been weighed: %.2f.\n", msg_received.person_id, msg_received.weight);
        else if (msg_received.input == LEFT_UNLOCK_I) printf("%s\n", outMessage[LEFT_UNLOCK_O]);
        else if (msg_received.input == RIGHT_UNLOCK_I) printf("%s\n", outMessage[RIGHT_UNLOCK_O]);
        else if (msg_received.input == LEFT_OPEN_I) printf("%s\n", outMessage[LEFT_OPEN_O]);
        else if (msg_received.input == RIGHT_OPEN_I) printf("%s\n", outMessage[RIGHT_OPEN_O]);
        else if (msg_received.input == LEFT_CLOSE_I) printf("%s\n", outMessage[LEFT_CLOSE_O]);
        else if (msg_received.input == RIGHT_CLOSE_I) printf("%s\n", outMessage[RIGHT_CLOSE_O]);
        else if (msg_received.input == LEFT_LOCK_I) printf("%s\n", outMessage[LEFT_LOCK_O]);
        else if (msg_received.input == RIGHT_LOCK_I) printf("%s\n", outMessage[RIGHT_LOCK_O]);
        //Print message to Neutrino console (i.e. stdout)
        else printf("%s\n", outMessage[msg_received.input]);

        //IF message == EXIT THEN
        if (msg_received.input == EXIT_I) break;
    }

    ChannelDestroy(chid);
    return EXIT_SUCCESS;

}
