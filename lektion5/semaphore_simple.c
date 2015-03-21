#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/sem.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#define SEMPERM 0600

int create_sem(key_t key) {
    int semaphore_id = semget(key, 1, IPC_CREAT | SEMPERM); /* handle_error(semaphore_id); */
    return semaphore_id;
}

int main(int argc, const char *argv[])
{
    int retcode;
    struct sembuf sem;

    key_t sem_key = ftok("./semref.dat", 1);
    if (sem_key < 0) {
        printf("ftok failed\n");
        exit(-1);
    }

    int semaphore_id = create_sem(sem_key);
    short semval[1];
    semval[0] = (short) 5;
    retcode = semctl(semaphore_id, 1, SETALL, &semval); /* handle_error(retcode); */

    int j;
    for (j = 0; j < 50; j++) {
        sem.sem_num = 0;
        sem.sem_op = -1;
        sem.sem_flg = SEM_UNDO;
        printf("%d obtaining resource\n", j);
        fflush(stdout);
        retcode = semop(semaphore_id, &sem, 1);
        if(retcode != 0) {
            printf("semop failed\n");
        }

    }
    exit(0);
}
