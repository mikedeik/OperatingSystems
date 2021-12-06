#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>

int main(int argc, char **argv)
{
  int id, err;
	int *mem;
  id = atoi(argv[1]);
  //n = atoi(argv[2]);
  
  printf(">> The Shared Segment Id is: %d\n", id);

	/* Attach the segment */
	mem = (int *) shmat(id, NULL,0);
	if (*(int *) mem == -1) {
		perror("Attachment.");
	} else {
		printf(">> Attached Shared Segment whose Mem content is: %d\n", *mem);
	}

  *mem = 4;
  printf("this is the child with pid %d printing shared memmory id %d  and value %d\n" ,getpid(),id,(*mem));


  /* Detach segment */
	err = shmdt((void *) mem);
	if (err == -1) {
		perror ("Detachment.");
	} else {
		printf(">> Detachment of Shared Segment %d\n", err);
	}

  
  return 0;
  
}