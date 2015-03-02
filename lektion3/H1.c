/* (C) IT Sky Consulting GmbH 2014
 * http://www.it-sky-consulting.com/
 * Author: Karl Brodowsky
 * Date: 2014-02-27
 * License: GPL v2 (See https://de.wikipedia.org/wiki/GNU_General_Public_License )
 */


#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/stat.h>

//#include "itskylib.h"

void handle_error(int retcode, char * message) {
    printf("Error: %s, ExitCode: %d", message, retcode);
    exit(retcode);
}

int main(int argc, char *argv[]) {

    int fork_result;
    char buff[1024];
    int retcode;
    int i;

    char* pipe1_file = "/tmp/pipe1";
    char* pipe2_file = "/tmp/pipe2";

    // create named pipes
    int state = mknod(pipe1_file, S_IFIFO | 0666, 0);
    if(state < 0)
        perror("mknod() error");

    state = mknod(pipe2_file, S_IFIFO | 0666, 0);
    if(state < 0)
        perror("mknod() error");

    int pipe1 = open(pipe1_file, O_RDONLY | O_NONBLOCK);
    int pipe2 = open(pipe2_file, O_RDONLY | O_NONBLOCK);

    /* message content is not relevant... */
    char message[] = "X";
    printf("Started parent\n");

    /* first fork() to create child */
    fork_result = fork();
    if (fork_result == 0) {
        printf("In child\n");

        /* second fork to create grand child */
        fork_result = fork();
        if (fork_result != 0) {
            /* exit child, make grand child a daemon */
            printf("exiting child\n");
            exit(0);
        }

        /* in grand child */
        /* close write end of pipe */
        retcode = close(pipe2);
        handle_error(retcode, "error closing pipe in grandchild (daemon)");

        /* read from pipe */
        printf("in grandchild: waiting for pipe\n");
        retcode = read(pipe1, buff, strlen(message));
        handle_error(retcode, "error reading from pipe in grandchild");

        printf("grandchild has received message from grand parent: daemonized\n");
        /* pipe has done its purpose, close it */
        retcode = close(pipe1);
        handle_error(retcode, "error closing pipe in daemon");

        printf("daemon has ppid=%d\n", getppid());

        /* do daemon stuff */
        sleep(30);
        printf("done with daemon\n");
        exit(0);
    }

    /* in parent */
    close(pipe1);
    int status;
    printf("parent waiting for child\n");
    wait(&status);
    printf("child terminated\n");
    retcode = write(pipe1, message, strlen(message));
    handle_error(retcode, "error writing to pipe in parent");

    retcode = close(pipe1);
    handle_error(retcode, "error closing pipe in parent");

    printf("parent done\n");
    unlink(pipe1_file);
    unlink(pipe2_file);
    exit(0);
}
