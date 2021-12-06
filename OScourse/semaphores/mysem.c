#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <semaphore.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>

#define CHILD_PROGRAM "./mysemchild"
#define SEM_NAME "semaphore_example"
#define SEM_PERMS (S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP)
#define INITIAL_VALUE 1


int main(int argc,char *argv[]){

    printf("this is the parent process \n");

    if (argc < 2)
    {
        printf("Missing Filename\n");
        return(1);
    }

    int count = 0;
    int lineno = 3;
    FILE *fp;
    char* filename;
    filename = argv[1];

    fp = fopen(filename, "r");

    char line[100];
    while (fgets(line, sizeof(line) ,fp) != NULL)
    {
        if (count == lineno)
        {
            printf("this is line %d of file : \n %s \n" ,count,line);
        }
        else
        {
            count++;
        }
    }

    fclose(fp);
    printf("file is closed \n");


    sem_t *semaphore = sem_open(SEM_NAME, O_CREAT | O_EXCL, SEM_PERMS, INITIAL_VALUE);


    if (semaphore == SEM_FAILED) {
        perror("sem_open(3) error");
        exit(EXIT_FAILURE);
    }

    pid_t pids[2];
    size_t i;

    for (i = 0; i < sizeof(pids)/sizeof(pids[0]); i++) {
        if ((pids[i] = fork()) < 0) {
            perror("fork(2) failed");
            exit(EXIT_FAILURE);
        }

        if (pids[i] == 0) {
            if (execl(CHILD_PROGRAM, CHILD_PROGRAM, NULL) < 0) {
                perror("execl(2) failed");
                exit(EXIT_FAILURE);
            }
            break;
        }
    }

    for (i = 0; i < sizeof(pids)/sizeof(pids[0]); i++)
        if (waitpid(pids[i], NULL, 0) < 0)
            perror("waitpid(2) failed");

    if (sem_unlink(SEM_NAME) < 0)
        perror("sem_unlink(3) failed");
    //sem_getvalue(semaphore,)

    return 0;
}