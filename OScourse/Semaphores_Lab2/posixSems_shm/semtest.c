/* semtest.c:  Posix Semaphore test example */

#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <sys/types.h>
#include <sys/ipc.h>

extern int errno;

int main(int argc, char **argv)
{
	sem_t sp;
	int retval;
    int whatisthevalue=0;

	/* Initialize the semaphore. */
	retval = sem_init(&sp, 1, 2);
	if (retval != 0) {
		perror("Couldn't initialize.");
		exit(3);
	}

    sem_getvalue(&sp, &whatisthevalue);
    printf("Init: Value of Semaphore is %d\n",whatisthevalue);
	retval = sem_wait(&sp);

	printf("Did wait. Returned %d > ",retval);
	getchar();
    sem_getvalue(&sp, &whatisthevalue);
    printf("Value of Semaphore is %d\n",whatisthevalue);

	retval = sem_trywait(&sp);
	printf("Did trywait. Returned %d > ",retval);
	getchar();
    sem_getvalue(&sp, &whatisthevalue);
    printf("Value of Semaphore is %d\n",whatisthevalue);

	retval = sem_trywait(&sp);
	printf("Did trywait. Returned %d > ",retval);
	getchar();
    sem_getvalue(&sp, &whatisthevalue);
    printf("Last: Value of Semaphore is %d\n",whatisthevalue);

	sem_destroy(&sp);
	return 0;
}
