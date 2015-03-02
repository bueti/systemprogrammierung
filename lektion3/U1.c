/**
 * Reads nonblocking from a pipe and prints what's in there
 *
 * Usage:
 *  create a pipe (mkfifo /tmp/my_pipe)
 *  (echo foo; sleep 5; echo bar; sleep 5; echo foobar;) > /tmp/my_pipe
 *
 *  Run this program: ./a.out /tmp/my_pipe
 */
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#define BUFFSIZE    4096

int main(int argc, const char *argv[]) {
    int fd, s;

    char buf[BUFFSIZE];

    if(argc == 2) {
        fd = open(argv[1], O_RDONLY | O_NONBLOCK);
        while(1) {
            s = read(fd, buf, BUFFSIZE);
            if(s == 0) // we are finished
                break;

            if (s == -1) { // nothing is in the pipe
                if(errno == EAGAIN) {
                    printf("\nwaiting for input\n");
                    sleep(2);
                } else {
                    exit(1);
                }
            }
            write(STDOUT_FILENO, buf, s);
        }
    } else {
        printf("%s: please specify file\n", argv[0]);
        exit(1);
    }

    return 0;
}
