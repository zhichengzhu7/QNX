#include <stdio.h>
#include <stdlib.h>
#include <process.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/neutrino.h>
#include "calc_message.h"
/**
 * Msg_Passing_Server.c
 *
 * Usage: ./calc_server
 *
 * Note the server's PID (required for client).
 *
 * @author Omar Metwally, Getting Started with QNX Neutrino 2, Message Passing
 * modified by metw0004@algonquinlive.com
 *
 * Read the //TODOS: Window menu > Show View > Tasks
 */
int main(int argc, char *argv[])
{
    int rcvid;        // indicates who we should reply to
    int chid;         // the channel ID
    char message[50]; // how big the message is
    client_send_t client_message;
    server_response_t response;

    // create a channel --- Phase I
    chid = ChannelCreate(0);
    if (chid == -1)
    {
        perror("failed to create the channel.");
        exit(EXIT_FAILURE);
    }
    printf("Server PID is %d", getpid());
    // server always running --- Phase II
    while (1)
    {
    	double left = client_message.left_hand;
    	double right = client_message.right_hand;
        // get the message, and print it
        //TODO (server) comment out && client will 'SEND' block on server
    	rcvid = MsgReceive( chid, &client_message, sizeof(client_message), NULL);
        switch(client_message.operator) {

        case '+':
        		response.answer = client_message.left_hand + client_message.right_hand;
        		break;
        case '-':
        		response.answer = client_message.left_hand - client_message.right_hand;
        		break;
        case 'x':
             	 response.answer = client_message.left_hand * client_message.right_hand;
                 break;
        case '/':
                response.answer = left / right;
                break;

            }

        snprintf(message, 50, "%.2f", response.answer);
        MsgReply(rcvid, EOK, message, sizeof(message));
    }

    // destroy the channel when done --- Phase III
    ChannelDestroy(chid);
    return EXIT_SUCCESS;
}
