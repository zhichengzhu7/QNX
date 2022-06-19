
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <fcntl.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/types.h>
#include <sys/wait.h>


void sigusr1_handler(int sig);//
volatile sig_atomic_t  usr1Happened = 0;
sem_t *sem;//pointer to a semaphore

void* childThread(void* arg)
{
	//print message that thread has been created and display thread ID
	printf("Child thread %d created\n",pthread_self());
	/*
	 * while true:
	 * wait on the semaphore
	 * print message the thread has unblocked; display thread ID
	 * sleep for 5 secs (simulates work being done by thread)
	 */
	while(1) {
		//wait on the semaphore
		int nStatus = sem_wait(sem);
		//print message the thread has unblocked; display thread ID
		printf("Child thread ID %d  has unblocked    status: %d \n", pthread_self(), nStatus);
		sleep(5);
	}
	return NULL;
}

int main(void) {

	struct sigaction sa;

	sa.sa_handler = sigusr1_handler;
	sa.sa_flags = 0; // or SA_RESTART
	sigemptyset(&sa.sa_mask);
	//Use  ‘sigemptyset()’ and ’sigaction()’ to
	//register a handler function to intercept SIGUSR1 signals;

	//on error: call perror and exit
	if (sigaction(SIGUSR1, &sa, NULL) == -1) {
		perror("sigaction");
		exit(1);
	}

	//Prompt User for number of threads to create
	int nThread;
	int i;
	printf("Enter the number of threads to create\n");
	scanf("%d", &nThread);
	/*
	 * call the function to open a named semaphore; this
	 * process is creating the semaphore; therefore, set the oflag to: O_CREAT.
	 * The 2 additional parameters are: S_IWOTH and 0 (zero)
	 */
	sem = sem_open("semOpen", O_CREAT, S_IWOTH, 0);

	pthread_attr_t attr;

	/*
	 * for nThreads:
	 * call the pthread function to initializes attr with all default thread attributes
	 * call the pthread function to create a new thread (params 1 & 4 are NULL)
	 * call the pthread function to destroy attr
	 */

	for(i = 0; i < nThread; i++) {
		pthread_attr_init(&attr);
		pthread_create(NULL, &attr, &childThread, NULL);
		pthread_attr_destroy(&attr);
	}
	/*
	 * loop continuously until integer flag is set to true
	 */
	while(usr1Happened != 1){
		;
	}
	//call sem_close function to close the semaphore
	sem_close(sem);

	//call sem_destroy function to destroy semaphore
	sem_destroy(sem);

	//call exit with SUCCESS // end of main
	return EXIT_SUCCESS;
}

void sigusr1_handler(int sig) {
	usr1Happened =1;
	/*printf("PID = %d : received and exiting now \n",getpid());*/
}

