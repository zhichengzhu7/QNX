#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>

void sigusr1_handler(int sig);

volatile sig_atomic_t  usr1Happened = 0;


int main(void) {



	int numChildren;
	pid_t childPid[numChildren];
	int i;
	int n = 10;
	struct sigaction sa;
	int rv;

	sa.sa_handler = sigusr1_handler;
	sa.sa_flags = 0; // or SA_RESTART
	sigemptyset(&sa.sa_mask);

	printf("Enter the number of children: \n ");
	scanf("%d", &numChildren);


	for(i = 0; i < numChildren; i++){
		childPid[numChildren] = fork();

		if (childPid[numChildren] == 0) {
			for(i = 0; i < numChildren; i++){

				childPid[i] = getppid();
				printf(" PID = %d: Child running...\n",getppid());

				while (usr1Happened == 0) {
					if (sigaction(SIGUSR1, &sa, NULL) == -1) {
						perror("sigaction");
						exit(1);
					}
				}
			}

			//	for(i = 0; i < numChildren; i++){
			//		pid = fork();
			//		if (pid==0){
			//			printf(" PID = %d: Child running...\n",getppid());
			//			while (usr1Happened == 0) {
			//				if (sigaction(SIGUSR1, &sa, NULL) == -1) {
			//					perror("sigaction");
			//					exit(1);
			//				}
			//				exit(1);
			//			}
			//		}
		}else if (childPid[numChildren] != 0) {

			printf("PID = %d : Parent running... \n",getppid());
			while (usr1Happened == 0) {
							if (sigaction(SIGUSR1, &sa, NULL) == -1) {
								perror("sigaction");
								exit(1);
							}
						}
			for (i = 0; i < numChildren; i++){
				waitpid(childPid[numChildren], NULL, 0);
				printf("PARENT: Child: %d returned value is: \n", i);

				while (usr1Happened == 0) {
					if (sigaction(SIGUSR1, &sa, NULL) == -1) {
						perror("sigaction");
						exit(1);
					}
					exit(1);
				}


				//make sure waited for children to be killed "wait()" function

				printf("PID = %d : Parent running... \n",getppid());


				printf("PID = %d :  Child exiting. \n",getpid());
				printf("PID = %d :  Children finished, parent exiting. \n",getpid());
				return 0;
			}
		}
	}
}

void sigusr1_handler(int sig) {
	usr1Happened =1;
	printf("PID = %d : Child received USR1 \n",getpid());
	printf("PID = %d : Child exiting. \n",getpid());

}
