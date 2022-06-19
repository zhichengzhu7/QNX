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

void sigusr1_handler(int sig);

volatile sig_atomic_t  usr1Happened = 0;


/*******************************************************************************
 * main( )
 ******************************************************************************/
int main(void) {
	pid_t pid[4];
	int numChildren;
	int i;
	struct sigaction sa;
	int rv;

	sa.sa_handler = sigusr1_handler;
	sa.sa_flags = 0; // or SA_RESTART
	sigemptyset(&sa.sa_mask);


	if (sigaction(SIGUSR1, &sa, NULL) == -1) {
			perror("sigaction");
			exit(1);
	}
	//run program from Momentics IDE
	printf("Enter the number of children: \n ");
	scanf("%d", &numChildren);
	printf("PID = %d : Parent running... \n",getppid());

	for(i = 0; i < numChildren; i++){
		if (pid[i]==0){
			printf(" PID = %d: Child running...\n",getppid());
			scanf(" %d", &rv);
			exit(rv);
		}else{
			perror("fork"); /* something went wrong */
			exit(1); /* parent exits */
		}
	}

	printf("PID = %d :  Child exiting. \n",getpid());
	printf("PID = %d :  Children finished, parent exiting. \n",getpid());
	return 0;
}

void sigusr1_handler(int sig) {
	usr1Happened =1;
	printf("PID = %d : Child received USR1 \n",getpid());
	printf("PID = %d : Child exiting. \n",getpid());

}
