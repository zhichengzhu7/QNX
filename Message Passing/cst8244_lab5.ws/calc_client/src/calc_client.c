#include <stdio.h>
#include <stdlib.h>
#include <process.h>
#include <sys/neutrino.h>
#include <sys/netmgr.h>
#include <string.h>
#include "../../calc_server/src/calc_message.h"
/**
 * Msg_Passing_Client.c
 *
 * Usage: ./calc_client PID_of_Server
 *
 * Edit the Launch Configuration for the client (gear icon) > Arguments tab > enter PID of server
 */
int main (int argc, char* argv[])
{
	client_send_t send_msg;
	char rmsg [200];
	int  coid;
	pid_t serverpid = atoi(argv[1]);
	send_msg.left_hand = atoi (argv[2]);
	send_msg.right_hand = atoi (argv[4]);

	char *op;
	op = argv[3];
	switch(*op) {
	case '+' :
		send_msg.operator = '+';
		break;
	case '-':
		send_msg.operator = '-';
		break;
	case 'x':
		send_msg.operator = 'x';
		break;
	case '/':
		send_msg.operator = '/';
		break;
	case '?':
		send_msg.operator = '?';
		break;
	}
	// establish a connection --- Phase I
	coid = ConnectAttach (ND_LOCAL_NODE, serverpid, 1, _NTO_SIDE_CHANNEL, 0);
	if (coid == -1) {
		fprintf (stderr, "Couldn't ConnectAttach\n");
		perror (NULL);
		exit (EXIT_FAILURE);
	}

	// send the message --- Phase II
	if (MsgSend (coid, &send_msg, sizeof (send_msg) + 1, rmsg, sizeof (rmsg)) == -1L) {
		fprintf (stderr, "Error during MsgSend\n");
		perror (NULL);
		exit (EXIT_FAILURE);
	}
	if((send_msg.operator == '/') && (send_msg.left_hand != 0) && (send_msg.right_hand == 0)){
		printf("\nError message from server: UNDEFINED: %d %c %d \n", send_msg.left_hand , send_msg.operator, send_msg.right_hand);
		exit(-1);
	}

	if((send_msg.operator == '?')){
		printf("\nError message from server: INVALID OPERATOR: %c \n", send_msg.operator);
		exit(-1);
	}

	if((send_msg.left_hand > 2147483647) && (send_msg.right_hand != 0) && (rmsg > 0)){
		printf("\nError message from server: OVERFLOW %d %c %d \n", send_msg.left_hand , send_msg.operator, send_msg.right_hand);
		exit(-1);
	}

	if ((strlen (rmsg) > 0) && (rmsg > 0)) {
		printf("The server has calculated the result of %d %c %d ",send_msg.left_hand , send_msg.operator, send_msg.right_hand);
		printf("as %s\n", rmsg);
	}
	//Disconnect from the channel --- Phase III
	ConnectDetach(coid);
	return EXIT_SUCCESS;
}
