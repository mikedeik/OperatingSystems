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
    // agrument inputs
    int K,N;            // K = number of child processes | N = number of iterations for each client
    K = atoi(argv[2]);    
    N = atoi(argv[3]);
    
    // shared mem handlers
    int mem_id = 0, err = 0;

    shared_mem * shmem;
    
    
    // init memmory

    mem_id = shmget(IPC_PRIVATE,sizeof(shared_mem),0666); 
	if (mem_id == -1) {
		perror ("Creation");
	} else {
		printf("Allocated Shared Memory with mem_ID: %d\n",(int)mem_id);
	}
    


    // atach memory segment
    shmem = shmat(mem_id, NULL, 0);
	if ( shmem == (void*)-1) {
		perror("Attachment.");
	} else {
		printf("Just Attached Shared Memory \n");
	}



    //init semaphores

    sem_t *sem_proc = sem_open(SEM_PROCESS, O_CREAT | O_EXCL, SEM_PERMS, INITIAL_VALUE);
    sem_t *sem_req = sem_open(SEM_REQUEST, O_CREAT | O_EXCL, SEM_PERMS, INITIAL_VALUE);
    sem_t *sem_clients = sem_open(SEM_CLIENTS, O_CREAT | O_EXCL, SEM_PERMS,1); // this is used only by the clients
    

    if (sem_proc == SEM_FAILED) {
        perror("sem_open(3) error");
        exit(EXIT_FAILURE);
    }
    if(sem_req == SEM_FAILED){
        perror("sem_open(3) error");
        exit(EXIT_FAILURE);
    }
    if(sem_clients == SEM_FAILED){
        perror("sem_open(3) error");
        exit(EXIT_FAILURE);
    }
    



    
    
    // file handling
    int linecount = 1;
    FILE *fp;
    char* filename;
    filename = argv[1];   
    fp = fopen(filename, "r");
    while (fgets((shmem->line), sizeof (shmem->line) ,fp) != NULL){
        printf(" this is line %d : %s \n" , linecount,shmem->line);
        linecount++;
    }

    int status;
    int value;
    //int forkvalue = fork();
    char sm[10]; // is this correct?
    char ln[10];
    size_t i;
    pid_t pids[K];
    
    sprintf(sm,"%d",mem_id);
    sprintf(ln,"%d",(linecount-1));


    for (i = 0; i <sizeof(pids)/sizeof(pids[0]) ; i++)
    {
        if ((pids[i] = fork()) < 0) {
            perror("fork(2) failed");
            exit(EXIT_FAILURE);
        }
        if (pids[i] == 0)
        {
            
            printf("trying to execl\n");
            (int) execl(CHILD_PROGRAM,CHILD_PROGRAM,sm,ln,argv[3],NULL);
            //exit(1);
        }
        else
        {        
            printf("building child processes\n");
        }
        
    }       
    for (int  j = 0; j < K*N; j++)
    {
        //sleep(1);
        sem_wait(sem_proc);
        printf("parent replying to request for line: %d \n" , shmem->lineNo);
        linecount = 1;
        rewind(fp); // this rewinds the file to the begining
        while (fgets(shmem->line, sizeof(shmem->line) ,fp) != NULL)
        {
            if(linecount == shmem->lineNo)
            {
                break;
                linecount++;
            }
            else
            {
                linecount++;
            }
        }
        sem_post(sem_req); 
    }

        
    

    for (i = 0; i < sizeof(pids)/sizeof(pids[0]); i++){
        if (waitpid(pids[i], NULL, 0) < 0)
            perror("waitpid(2) failed");

    }

    fclose(fp);

    getchar();


    //unlink semaphores
    if (sem_unlink(SEM_PROCESS) < 0) perror("sem_unlink(3) failed");


    if (sem_unlink(SEM_REQUEST) < 0) perror("sem_unlink(3) failed");

    if (sem_unlink(SEM_CLIENTS) <0) perror("sem_unlink(3) failed");

    // detach memmory segment

    err = shmdt((shared_mem *) shmem);
	if (err == -1) {
		perror ("Detachment.");
	} else {
		printf(">> Detachment of Shared Segment %d\n", err);
	}

    // removing memory segment
    err = shmctl(mem_id, IPC_RMID, 0);
	if (err == -1) {
		perror ("Removal.");
	} else {
		printf("Just Removed Shared Segment. %d\n", (int)(err));
	}


    return 0;
}