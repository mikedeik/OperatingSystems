#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <semaphore.h>
#include <fcntl.h>
#include <signal.h>
#include <time.h>
#include "defines.h"






int main(int argc, char * argv[]){


    // checking arguments
    if (argc<4)
    {
        printf("error input arguments");
        exit(EXIT_FAILURE);
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
        exit(EXIT_FAILURE);
	}
    
    


    // atach memory segment
    shmem = shmat(mem_id, NULL, 0);
	if ( shmem == (void*)-1) {
		perror("Attachment.");
        exit(EXIT_FAILURE);
	}
    
    if (sem_unlink(SEM_PROCESS) < 0) perror("sem_unlink(3) failed");


    if (sem_unlink(SEM_REQUEST) < 0) perror("sem_unlink(3) failed");

    if (sem_unlink(SEM_CLIENTS) <0) perror("sem_unlink(3) failed");

    if (sem_unlink(SEM_DONE) <0) perror("sem_unlink(3) failed"); 


    //init semaphores

    sem_t *sem_proc = sem_open(SEM_PROCESS, O_CREAT | O_EXCL, SEM_PERMS, INITIAL_VALUE); //those 2 semaphores will handle the request/reply part
    sem_t *sem_req = sem_open(SEM_REQUEST, O_CREAT | O_EXCL, SEM_PERMS, INITIAL_VALUE);

    sem_t *sem_clients = sem_open(SEM_CLIENTS, O_CREAT | O_EXCL, SEM_PERMS,INITIAL_VALUE); // those 2 semaphores handle the entry/leave the 
    sem_t *sem_done = sem_open(SEM_DONE, O_CREAT | O_EXCL, SEM_PERMS,INITIAL_VALUE);       // shared memory section part
    

    if (sem_proc == SEM_FAILED) {                           // checking that all semaphores were created with no error
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
    if(sem_done == SEM_FAILED){
        perror("sem_open(3) error");
        exit(EXIT_FAILURE);
    }
    

    // file handling   and  counting the lines of the file // 
    int linecount = 1;      
    FILE *fp;
    char* filename;
    filename = argv[1];   
    fp = fopen(filename, "r");
    while (fgets((shmem->line), sizeof (shmem->line) ,fp) != NULL){
        linecount++;
    }

    

    



    
    char sm[10]; // we will set the mem_id here as char to pass to child processes
    char ln[10]; // this is the linecount (biggest int has 10 digits in 32-bit)
    
    size_t i;
    pid_t pids[K]; // we will store the pids of K Children here 
    
    sprintf(sm,"%d",mem_id); // setting both values as char[] to be able to pass as arguments
    sprintf(ln,"%d",(linecount-1));





    for (i = 0; i <sizeof(pids)/sizeof(pids[0]) ; i++) // we fork K times but just to be safe we use sizeof(pids)/sizeof(1 pid)
    {
        if ((pids[i] = fork()) < 0) {
            perror("fork(2) failed");
            exit(EXIT_FAILURE);       // if any fork fails we exit(1)
        }
        if (pids[i] == 0)
        {
            
            (int) execl(CHILD_PROGRAM,CHILD_PROGRAM,sm,ln,argv[3],NULL); // we call execl so the child processes will run ./client

            kill(pids[i],SIGKILL);                                      // if execl fails we signal-kill the child process wich will exit with value 1
            exit(EXIT_FAILURE);
        }
        
        
    }   
    struct timespec ts;                    // this part is used for sem_timedwait() 
    if (clock_gettime(CLOCK_REALTIME,&ts)<0)
    {
        exit(EXIT_FAILURE);
    }
    
    ts.tv_sec += 60;                        // we init to 5 secs 
    

    /******************** THIS IS THE TRANSACTION PART *******************************/
    int counter = 0;

    while(1)        // keep getting requests                  
    {
        
        
        if(sem_post(sem_clients)<0){         // Give a child access to semaphore for critical section
            perror("sem_post(3) clients failed on parrent");
            continue;

        } 

        /************************* PARENT-CHILD ACTION IN CRITICAL SECTION ***************************************/

        printf("Parent waiting for request \n");
        if(sem_timedwait(sem_proc,&ts)<0){          // timed wait here for 5 secs. if no child is active this will break the loop and terminate the server program
            printf("All requests are done \n");     // if children are active they will post the semaphore after sending the request 
            break;
        }

        printf("parent replying to request for line: %d \n" , shmem->lineNo);
        linecount = 1;
        rewind(fp); // this rewinds the file to the begining
        while (fgets(shmem->line, sizeof(shmem->line) ,fp) != NULL)
        {
            if(linecount == shmem->lineNo)
            {
                break;  // get requested line
                linecount++;
            }
            else
            {
                linecount++;
            }
        }

        if(sem_post(sem_req)<0){                             // posting the request semaphore so child can print the line
            perror("sem_post(3) req failed on parent ");
            continue;
        } 
        /************************* PARENT-CHILD ACTION IN CRITICAL SECTION OVER ***************************************/

        
        if(sem_wait(sem_done)<0){            // waits for current child to finish printing
            perror("sem_wait(3) done failed on parent");
            continue;
        }
        counter++;
    }

    // we wait the children processes to get their exit status
    
    for (i = 0; i < sizeof(pids)/sizeof(pids[0]); i++){
        if (waitpid(pids[i], NULL, 0) < 0)
            perror("waitpid(2) failed");

    }
    
    fclose(fp);     // after we are done we close the file

    printf("the counter is %d \n", counter);



    //  unlink semaphores
    if (sem_unlink(SEM_PROCESS) < 0) perror("sem_unlink(3) failed");


    if (sem_unlink(SEM_REQUEST) < 0) perror("sem_unlink(3) failed");

    if (sem_unlink(SEM_CLIENTS) <0) perror("sem_unlink(3) failed");

    if (sem_unlink(SEM_DONE) <0) perror("sem_unlink(3) failed");

    

    // detach memmory segment

    err = shmdt((shared_mem *) shmem);
	if (err == -1) {
		perror ("Detachment.");
	}
    
    // removing memory segment
    err = shmctl(mem_id, IPC_RMID, 0);
	if (err == -1) {
		perror ("Removal.");
	} 
    



    return 0;
}