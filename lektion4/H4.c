/* original copyright:
 * (C) IT Sky Consulting GmbH 2014
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
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>

int daemonized = 0;

void signal_handler(int signo) {
    daemonized = 1;
    int pid = getpid();
    int pgid = getpgid(pid);
    int ppid = getppid();
    printf("signal %d received pid=%d pgid=%d ppid=%d\n", signo, pid, pgid, ppid);

}

void signal_handler2(int signo) {
    daemonized = 1;
    int pid = getpid();
    int pgid = getpgid(pid);
    int ppid = getppid();
    printf("grandparent received confirmation (grandchild daemonized successfully) %d received pid=%d pgid=%d ppid=%d\n", signo, pid, pgid, ppid);
}

void signal_handler3(int signo) {
    printf("signal %d in grandparent received..do not care\n",signo);
}

void handle_error(int retcode, char* msg ){
    if( retcode < 0 ){
        printf("%s", msg );
        exit(1);
    }
}

int main(int argc, char *argv[]) {
    int retcode;

    int fork_result;
    int status;
    int pid = getpid();
    int pgid = pid;

    /* set pgid to pid */
    retcode = setpgid(pid, pgid);
    handle_error(retcode, "setpgid");
    printf("set pgid %d\n", pgid);

    signal(SIGUSR1, signal_handler3);

    printf("add sign handler2\n");
    /* set signal handler */
    if (signal(SIGUSR2, signal_handler2) == SIG_ERR) {
        fputs("An error occurred while setting a signal handler.\n", stderr);
        return EXIT_FAILURE;
    }

    /* first fork() to create child */
    fork_result = fork();
    handle_error(fork_result, "fork (1)");
    if (fork_result == 0) {
        printf("In child\n");

        /* set signal handler */
        if (signal(SIGUSR1, signal_handler) == SIG_ERR) {
            fputs("An error occurred while setting a signal handler.\n", stderr);
            return EXIT_FAILURE;
        }

         /* remove signal handler */
        if (signal(SIGUSR2, SIG_DFL) == SIG_ERR) {
            fputs("An error occurred while setting a signal handler.\n", stderr);
            return EXIT_FAILURE;
        }

        /* second fork to create grand child */
        fork_result = fork();
        handle_error(fork_result, "fork (2)");
        if (fork_result != 0) {
            /* exit child, make grand child a daemon */
            printf("exiting child\n");
            exit(0);
        }

        /* in daemon (grand child) */
        pid = getpid();

        while (! daemonized) {
            pause();
        }

        printf("daemon has pid=%d pgid=%d ppid=%d\n", pid, getpgid(pid), getppid());

        sleep(4);
        retcode = kill(-getpgid(pid), SIGUSR2);
        handle_error(retcode, "kill failed");

        /* do daemon stuff */
        sleep(2);
        printf("done with daemon\n");
        exit(0);
    }

    printf("grandparent waiting for child\n");
    retcode = wait(&status);
    handle_error(retcode, "wait");

    printf("child terminated status=%d\n", status);

    retcode = kill(-pgid, SIGUSR1);
    handle_error(retcode, "kill");

    //wait till daemon successfully created and sends confirmation back to grandparent
    while (! daemonized) {
        pause();
    }

    printf("grandparent done\n");
    exit(0);
}
