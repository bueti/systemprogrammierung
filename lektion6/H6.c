/* (C) IT Sky Consulting GmbH 2014
 * http://www.it-sky-consulting.com/
 * Author: Karl Brodowsky
 * Date: 2014-02-27
 * License: GPL v2 (See https://de.wikipedia.org/wiki/GNU_General_Public_License )
 */

/* use mutex for IPC, but do preparation separately */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <pthread.h>

#define BUF_SIZE 16384

pthread_mutex_t mutex;

void *run(void *arg) {
    int retcode;
    char buffer[BUF_SIZE];

    int *argi = (int *) arg;
    int my_tidx = *argi;

    retcode = pthread_mutex_lock(&mutex);
    // handle_thread_error(retcode, "child failed (timed)lock", PROCESS_EXIT);
    printf("%d: in blocking thread: got mutex\n", my_tidx);

    char line[1025];
    printf("%s", fgets(line, 1024, stdin));

    printf("%d: in blocking thread: releasing mutex\n", my_tidx);
    pthread_mutex_unlock(&mutex);
    printf("%d: in blocking thread: released mutex\n", my_tidx);
    printf("%d: in blocking thread: exiting\n", my_tidx);
    return NULL;
}

int main(int argc, char *argv[]) {

    int retcode;

    size_t N_THREADS = argc - 1;

    printf("in parent: setting up %ld threads\n", N_THREADS);

    pthread_t thread[N_THREADS];

    int tidx[N_THREADS];
    for (size_t i = 0; i < N_THREADS; i++) {
        tidx[i] = (int) i;
        retcode = pthread_create(thread + i, NULL, run, (void *) (tidx + i));
    }

    pthread_mutex_init(&mutex, NULL);
    sleep(2);

    printf("in parent: waiting for threads to finish\n");
    for (size_t i = 0; i < N_THREADS; i++) {
        retcode = pthread_join(thread[i], NULL);
        //handle_thread_error(retcode, "join failed", PROCESS_EXIT);
        printf("joined thread %d\n", (int) i);
    }
    pthread_mutex_destroy(&mutex);
    printf("done\n");
    exit(0);
}

