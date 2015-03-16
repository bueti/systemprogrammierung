#include <stdio.h>
#include <signal.h>
#include <unistd.h>

void sig_handler(int signo)
{
  if (signo == SIGINT)
    printf("received SIGINT\n");
  else if (signo == SIGSTOP)
    printf("received SIGSTOP\n");
}

int main(int argc, const char *argv[])
{

    pid_t fork_pid;

    fork_pid = fork();
    if (fork_pid == 0) {
        printf("we are in child\n");
        if (signal(SIGINT, sig_handler) == SIG_ERR)
          printf("\ncan't catch SIGINT\n");

        if (signal(SIGTSTP, sig_handler) == SIG_ERR)
          printf("\ncan't catch SIGTSTP\n");

        while(1)
            sleep(1);
    } else {
        printf("sending sigint\n");
        kill(fork_pid, SIGINT);
    }


    return 0;

}
