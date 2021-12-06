/* This is just like exec2.c, but it uses "cat" as the pathname instead of "bin/cat". */

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#define CHILD_NAME "./child"


int main(int argc, char **argv)
{



  int status,count;
 
  count = 2;
  int id=0, err=0;
	int *mem;
  char sid[5];

	/* Make shared memory segment */
	id = shmget(IPC_PRIVATE,10,0666); 
	if (id == -1) {
		perror ("Creation");
	} else {
		printf("Allocated Shared Memory with ID: %d\n",(int)id);
	}

	/* Attach the segment */
	mem = (int *) shmat(id, NULL, 0);
	if ( *(int *) mem == -1) {
		perror("Attachment.");
	} else {
		printf("Just Attached Shared Memory whose content is: %d\n",*mem);
	}


  *mem = 1;
  

  sprintf(sid,"%d",id);
  if (fork()==0)
  {
    //printf("changing file, pid is %d \n", getpid());
    (int) execl(CHILD_NAME,CHILD_NAME,sid,NULL);
  }
  else
  {
    //sleep(2);
    wait(&status);
  }
    
    
  /* Print out new value */
	printf("Content of Shared Mem is now: %d\n", *mem);
  getchar();


  /* Detach segment */
	err = shmdt((void *) mem);
	if (err == -1) {
		perror ("Detachment.");
	} else {
		printf(">> Detachment of Shared Segment %d\n", err);
	}

	/* Remove segment */
	err = shmctl(id, IPC_RMID, 0);
	if (err == -1) {
		perror ("Removal.");
	} else {
		printf("Just Removed Shared Segment. %d\n", (int)(err));
	}
  return 0;
  
}
