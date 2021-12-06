#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <semaphore.h>
#include <fcntl.h>
#include "defines.h"
#include <string.h>





int main(int argc, char * argv[]){



    if (argc<4)
    {
        printf("error input arguments");
        return -1;
    }
    // agrv inputs
    int K,N;
    K = atoi(argv[2]);    
    N = atoi(argv[3]);
    
    // shared mem handlers
    int id = 0, err = 0;

    shared_mem * shmem;
    
    
    // init memmory

    id = shmget(IPC_PRIVATE,sizeof(shared_mem),0666); 
	if (id == -1) {
		perror ("Creation");
	} else {
		printf("Allocated Shared Memory with ID: %d\n",(int)id);
	}
    
    //printf("this is the line string %s \n", line);

    // atach memory segment
    shmem = shmat(id, NULL, 0);
	if ( shmem == (void*)-1) {
		perror("Attachment.");
	} else {
		printf("Just Attached Shared Memory \n");
	}

    //init semaphores

    sem_t *semproc = sem_open(SEM_PROCESS, O_CREAT | O_EXCL, SEM_PERMS, INITIAL_VALUE);
    sem_t *semreq = sem_open(SEM_REQUEST, O_CREAT | O_EXCL, SEM_PERMS, INITIAL_VALUE);

    if (semproc == SEM_FAILED || semreq == SEM_FAILED) {
        perror("sem_open(3) error");
        exit(EXIT_FAILURE);
    }



    if (fork()==0)
    {
        execl(CHILD_PROGRAM,CHILD_PROGRAM,)
    }
    else
    {
        /* code */
    }
    
    
    // file handling
    int linecount = 1;
    FILE *fp;
    char* filename;
    filename = argv[1];   
    fp = fopen(filename, "r");
    while (fgets(shmem->line, sizeof shmem->line ,fp) != NULL){
        if(linecount == 1){
            printf("this is line 3 : %s \n" , shmem->line);
            linecount++;
        }
        else{
            linecount++;
        }
    }

    printf("there are %d lines \n" , linecount-1);


    fclose(fp);

    getchar();


    // detach memmory segment

    err = shmdt((shared_mem *) shmem);
	if (err == -1) {
		perror ("Detachment.");
	} else {
		printf(">> Detachment of Shared Segment %d\n", err);
	}

    // removing memory segment
    err = shmctl(id, IPC_RMID, 0);
	if (err == -1) {
		perror ("Removal.");
	} else {
		printf("Just Removed Shared Segment. %d\n", (int)(err));
	}


    return 0;
}