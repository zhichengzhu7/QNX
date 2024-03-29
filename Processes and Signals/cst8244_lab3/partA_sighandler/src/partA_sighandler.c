/*
 ** sigint.c -- grabs SIGINT
 *
 * Read:	http://beej.us/guide/bgipc/html/single/bgipc.html#signals
 * Source:	http://beej.us/guide/bgipc/examples/sigint.c
 *
 * Modified by: hurdleg@algonquincollege.com
 *
 * Usage:
 *  From Momentics IDE, run program; notice PID; enter some text, but don't hit the enter key
 *  At Neutrino prompt, issue the command: kill -s SIGINT <PID>
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>

void sigusr1_handler(int sig);//

volatile sig_atomic_t  usr1Happened = 0;


/*******************************************************************************
 * main( )
 ******************************************************************************/
int main(void) {


	struct sigaction sa;

	sa.sa_handler = sigusr1_handler;
	sa.sa_flags = 0; // or SA_RESTART
	sigemptyset(&sa.sa_mask);

	printf("PID = %d : Running...\n", getpid());

	while(usr1Happened ==0)	{
		if (sigaction(SIGUSR1, &sa, NULL) == -1) {
			perror("sigaction");
			exit(1);
		}
	}
	printf("PID = %d: Exiting. \n",getpid());
	return 0;
}

void sigusr1_handler(int sig) {
	usr1Happened =1;

	printf("PID = %d : Received USR1 \n",getpid());


}
