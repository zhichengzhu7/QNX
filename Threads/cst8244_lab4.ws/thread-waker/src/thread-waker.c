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


sem_t *sem;//pointer to a semaphore


int main(void) {
	/*
	 * call the function to open a semaphore;
	 * print the semaphore has already been created
	 * so only 2 prams are needed: nameOfYourSemaphore and 0 (zero)
	 */
	sem = sem_open("semOpen", 0);
	int numWakeup = 0;
	int y;
	//display the PID for the thread-waker process
	printf("Thread-Waker PID is %d \n", getpid());
	/*
	 *
	 * do-while numWakeup != 0:
	 * prompt User for the number of threads to wake-up
	 * call the function "numWakeup" to increment the count of the semaphore
	 */

	do{
		printf("Enter the number of threads to weak up: (Enter 0 to exit)\n");
		scanf("%d", &numWakeup);
		for(y = 0; y < numWakeup; y++) {
			sem_post(sem);
		}
	}while(numWakeup != 0);
	//call function to close the semaphore
	sem_close(sem);
	return EXIT_SUCCESS;
}
