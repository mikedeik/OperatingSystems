#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <time.h>
#include <fcntl.h>
#include <semaphore.h>
#include "defines.h"

int main(int argc, char *argv[])
{   


    //checking arguments
    if (argc<4)
    {
        perror("error in arguments \n");
        return -1;
    }
    
    
    int mem_id,err,totallines,requests;
    shared_mem *shmem;

    mem_id = atoi(argv[1]);
    totallines = atoi(argv[2]);
    requests = atoi(argv[3]);
    
    
    srand(getpid());

    

  


	/* Attach the memory segment */
	shmem = shmat(mem_id, NULL, 0);
	if ( shmem == (void*)-1) {
		perror("Attachment.");
	}
    //else {
	//	printf("Just Attached Shared Memory \n");
	//}

    

    // Attach semaphore
    sem_t *sem_req = sem_open(SEM_REQUEST,O_RDWR);
    if (sem_req == SEM_FAILED) {
        perror("sem_open(3) failed");
        exit(EXIT_FAILURE);
    }

    sem_t *sem_proc = sem_open(SEM_PROCESS,O_RDWR);
    if (sem_proc == SEM_FAILED) {
        perror("sem_open(3) failed");
        exit(EXIT_FAILURE);
    }
    sem_t *sem_clients = sem_open(SEM_CLIENTS,O_RDWR);
    if (sem_clients == SEM_FAILED) {
        perror("sem_open(3) failed");
        exit(EXIT_FAILURE);
    }

    sem_t *sem_done = sem_open(SEM_DONE,O_RDWR);
    if (sem_done == SEM_FAILED) {
        perror("sem_open(3) failed");
        exit(EXIT_FAILURE);
    }


    clock_t t;
    double result = 0.0;

    for (int i = 0; i < requests; i++)
    {
        int random = rand();
        

        printf("child with pid %d waiting to enter cirtical section \n ", getpid());

        // entering critical section
        if (sem_wait(sem_clients) < 0) {
           perror("sem_wait(3) failed on child");
           continue;
        }
        
        
        
        shmem->lineNo = (random % totallines + 1);
        printf("PID %d requesting line %d \n",getpid(), shmem->lineNo );

    /*********************************************************************/
        if (sem_post(sem_proc)<0)
        {
            perror("sem_post(3) failed on child");
            continue;
        }

        t = clock();

        if (sem_wait(sem_req) < 0) {
            perror("sem_wait(3) failed on child");
            continue;
        }

        
        printf("PID %d ,printing line %d from shared mem : %s \n",getpid(), shmem->lineNo , shmem->line);
        
        /*********************************************************************/

        printf("PID %d is done with request \n", getpid());

        sem_post(sem_done);
        


        result += (double)((clock()-t)/CLOCKS_PER_SEC);
        
        
        
    }

    double medium = result;

    printf("PID %d has medium response time for a request  %e \n" ,getpid(), medium);
    
    //close semaphore
    if (sem_close(sem_req) < 0){
        perror("sem_close(3) failed");
        exit(EXIT_FAILURE);
    }
    if (sem_close(sem_proc) < 0){
        perror("sem_close(3) failed");
        exit(EXIT_FAILURE);
    }

    if (sem_close(sem_clients) < 0){
        perror("sem_close(3) failed");
        exit(EXIT_FAILURE);
    }

    if (sem_close(sem_done) < 0){
        perror("sem_close(3) failed");
        exit(EXIT_FAILURE);
    }


    /* Detach segment */
	err = shmdt((shared_mem *) shmem);
	if (err == -1) {
		perror ("Detachment.");
	}
    //else {
	//	printf(">> Detachment of Shared Segment %d\n", err);
	//}

  
  return 0;
  
}