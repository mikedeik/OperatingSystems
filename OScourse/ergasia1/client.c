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
        exit(EXIT_FAILURE);
    }
    
    // we declare the variables we need
    int mem_id,err,totallines,requests;
    shared_mem *shmem;

    // we assign the values passed by arguments
    mem_id = atoi(argv[1]);
    totallines = atoi(argv[2]);
    requests = atoi(argv[3]);
    
    // we will use rand for the lines
    srand(getpid());



	/* Attach the memory segment */
	shmem = shmat(mem_id, NULL, 0);
	if ( shmem == (void*)-1) {
		perror("Attachment.");
        exit(EXIT_FAILURE);
	}
    

    

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

    // we need clock_t to check the time each request needs
    clock_t t;
    double result = 0.0;


    // we loop every request

    for (int i = 0; i < requests; i++)
    {
        int random = rand();
        

        printf("child with pid %d waiting to enter cirtical section \n ", getpid());

        // waiting post from parent to enter critical section
        if (sem_wait(sem_clients) < 0) {
           perror("sem_wait(3) clients failed on child");
           continue;
        }
        
        /************************* ENTERING CRITICAL SECTION ********************************************/
        
        shmem->lineNo = (random % totallines + 1);  // line request
        printf("PID %d requesting line %d \n",getpid(), shmem->lineNo );

    
        if (sem_post(sem_proc)<0)       // informs parent the request is sent
        {
            perror("sem_post(3) proc failed on child");
            continue;
        }

        t = clock();

        if (sem_wait(sem_req) < 0) {        // waiting for parent to reply
            perror("sem_wait(3) req failed on child");
            continue;
        }

        // printing from shared memory

        printf("PID %d ,printing line %d from shared mem : %s \n",getpid(), shmem->lineNo , shmem->line);
        
        
        
        /**************************** EXITING CRITICAL SECTION *****************************************/


        if(sem_post(sem_done)<0){    // informing parent that this request is done
            perror("sem_post(3) done failed on child");
            continue;
        }
        


        result += (double)((clock()-t)/CLOCKS_PER_SEC); 
        
        
        
    }

    double medium = result/requests; // getting the medium responce time

    printf("PID %d has medium response time for a request  %e \n" ,getpid(), medium);


    
    //closing semaphores
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
        exit(EXIT_FAILURE);
    }
    
    printf("THE CLIENT %d IS DONE!!!\n" , getpid());
    
    return 0;
  
}