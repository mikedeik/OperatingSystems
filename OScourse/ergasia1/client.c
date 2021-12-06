#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include "defines.h"

int main(int argc, char *argv[])
{   


    //checking arguments
    if (argc<5)
    {
        perror("error in arguments \n");
        return -1;
    }
    
    int mem_id,sem_id err,lineNo,requests;
    shared_mem *shmem;

    mem_id = atoi(argv[1]);
    sem_id = atoi(argv[2]);
    lineNo = atoi(argv[3]);
    requests = atoi(argv[4]);


  
    printf(">> The Shared Segment Id is: %d\n", id);

	/* Attach the memory segment */
	shmem = shmat(mem_id, NULL, 0);
	if ( shmem == (void*)-1) {
		perror("Attachment.");
	} else {
		printf("Just Attached Shared Memory \n");
	}

    // Attach semaphore
    sem_t *sem_req = sem_open(SEM_REQUEST, O_RDWR);
    if (sem_req == SEM_FAILED) {
        perror("sem_open(3) failed");
        exit(EXIT_FAILURE);
    }




    for (int i = 0; i < requests; i++)
    {
        if (sem_wait(sem_req) < 0) {
            perror("sem_wait(3) failed on child");
            continue;
        }
        

        shmem->lineNo = i % lineNo + 1;
    }
    



    /* Detach segment */
	err = shmdt((shared_mem *) shmem);
	if (err == -1) {
		perror ("Detachment.");
	} else {
		printf(">> Detachment of Shared Segment %d\n", err);
	}

  
  return 0;
  
}