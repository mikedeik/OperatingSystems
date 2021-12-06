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
    int mem_id = 0, err = 0;

    shared_mem * shmem;
    
    
    // init memmory

    mem_id = shmget(IPC_PRIVATE,sizeof(shared_mem),0666); 
	if (mem_id == -1) {
		perror ("Creation");
	} else {
		printf("Allocated Shared Memory with mem_ID: %d\n",(int)mem_id);
	}
    
    //unlink semaphores
    if (sem_unlink(SEM_PROCESS) < 0) perror("sem_unlink(3) failed");


    if (sem_unlink(SEM_REQUEST) < 0) perror("sem_unlink(3) failed");
//
    if (sem_unlink(SEM_TRY) < 0) perror("sem_unlink(3) failed");
    //printf("this is the line string %s \n", line);

    // atach memory segment
    shmem = shmat(mem_id, NULL, 0);
	if ( shmem == (void*)-1) {
		perror("Attachment.");
	} else {
		printf("Just Attached Shared Memory \n");
	}

    //init semaphores

    sem_t *semproc = sem_open(SEM_PROCESS, O_CREAT | O_EXCL, SEM_PERMS, INITIAL_VALUE);
    sem_t *semreq = sem_open(SEM_REQUEST, O_CREAT | O_EXCL, SEM_PERMS, INITIAL_VALUE);
    sem_t *semtry = sem_open(SEM_TRY , O_CREAT | O_EXCL, SEM_PERMS, INITIAL_VALUE);

    if (semproc == SEM_FAILED || semreq == SEM_FAILED) {
        perror("sem_open(3) error");
        exit(EXIT_FAILURE);
    }
    else
    {
        printf("both semaphores are open\n");
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
    int forkvalue = fork();
    char sm[20];
    char ln[20];
    printf("this is forkvalue %d \n", forkvalue);
    sprintf(sm,"%d",mem_id);
    sprintf(ln,"%d",(linecount-1));
    
    if (forkvalue == 0)
    {
        printf("trying to execl\n");
        (int) execl(CHILD_PROGRAM,CHILD_PROGRAM,sm,ln,argv[3],NULL);
        //exit(1);
    }
    else
    {

        printf("this is parent process\n");

        //sem_wait(semtry);
        
        for (int  i = 0; i < N; i++)
        {
            //sleep(1);
            sem_wait(semproc);

            //sem_wait(semreq);
            printf("parent replying to request for line: %d \n" , shmem->lineNo);


            

            linecount = 1;
            rewind(fp); // this rewinds the file to the begining
            while (fgets(shmem->line, sizeof(shmem->line) ,fp) != NULL)
            {

                if(linecount == shmem->lineNo)
                {
                    printf("correct line count \n");
                    break;
                    linecount++;
                }
                else
                {
                    printf("add 1 line \n");
                    linecount++;
                }
            }
            

            sem_post(semreq);
            sem_post(semproc);

            
            
            
            
            
            sem_wait(semtry);
             
        }
        
           

        

        wait(&status);
    }


    
    

    //printf("there are %d lines \n" , linecount-1);


    fclose(fp);

    getchar();


    //unlink semaphores
    if (sem_unlink(SEM_PROCESS) < 0) perror("sem_unlink(3) failed");


    if (sem_unlink(SEM_REQUEST) < 0) perror("sem_unlink(3) failed");

    if (sem_unlink(SEM_TRY) < 0) perror("sem_unlink(3) failed");

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