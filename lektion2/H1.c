#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>

#define BUFFSIZE    4096

void signal_callback_handler(int signum) {
    printf("Caught signal %d\n",signum);
    exit(signum);
}

int wait_for_input() {
    int view = 1;
    int ch;

    while (view){
        ch = getchar();
        if( ch ) view = 0;
    }

    return ch;
}

void handle_input(int input) {
    printf("you pressed: %d\n", input);
}

static void read_file(const char *filename) {
    // Signal handler
    signal(SIGINT, signal_callback_handler);

    int f,n,leave,rdlen;
    char buf[BUFFSIZE];

    struct termios termios_struct,termios_struct_orig;

    tcgetattr(STDIN_FILENO, &termios_struct_orig);
    tcgetattr(STDIN_FILENO, &termios_struct);
    termios_struct.c_lflag &= (~ICANON) & (~ECHO);
    termios_struct.c_cc[VTIME] = 0;
    termios_struct.c_cc[VMIN] = 1;
    tcsetattr(STDIN_FILENO, TCSANOW, &termios_struct);

    f = open(filename, O_RDONLY);
    if( f < 0 ) {
      perror("Error while opening the file.\n");
      exit(EXIT_FAILURE);
    }

    for (leave= 0; ! leave; ) {
        rdlen= read (f, buf, BUFFSIZE);
        if (rdlen<0) {
            perror ("read(0)");
            leave= 1; /* error */
            continue;
        } else if (rdlen==0) {
            leave= 1; /* EOF */
            continue;
        }

        write(STDOUT_FILENO, buf, rdlen);

        handle_input(wait_for_input());

        if (buf[0]==4) leave= 1; /* ctrl-D */
    }

    tcsetattr(STDIN_FILENO, TCSANOW, &termios_struct_orig);

}

int main(int argc, const char *argv[]) {

    if (argc > 1) {
        read_file(argv[1]);
        handle_input(wait_for_input());
    } else {
        // handle stdin
        printf("reading from STDIN_FILENO doesnt properly work.\n");
        exit(0);
        while(1) {
            char c;
            int b = read(STDIN_FILENO, &c, BUFFSIZE);
            if (b < 1) {
                printf("nothing read\n");
            }
            printf("%c", c);
            fflush(stdout);
            wait_for_input();
            fflush(stdout);
        }
    }

}
