#define CHILD_PROGRAM "./client"
#define SEM_PROCESS "sem_process"
#define SEM_REQUEST "sem_request"
#define SEM_TRY "sem_try"
#define SEM_PERMS (S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP)
#define INITIAL_VALUE 0

typedef struct memory_struct
{
    int lineNo;
    char line[100];
}shared_mem;

