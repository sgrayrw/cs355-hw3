#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

void chldhandler(int);

int main() {
    // initialize sigset to be empty
    sigset_t sigset;
    sigemptyset(&sigset);

    // register for SIGCHLD
    signal(SIGCHLD, chldhandler);

    pid_t pid;
    if ((pid = fork()) == 0) {
        // child, terminate in 1 sec
        sleep(1);
        printf("Child terminated now\n");
        exit(0);
    } else {
        // parent, enter critical region not to be interrupted by SIGCHLD
        sigaddset(&sigset, SIGCHLD);
        sigprocmask(SIG_BLOCK, &sigset, NULL);

        printf("********ENTERING CRITICAL REGION********\n");
        sleep(3);
        printf("********EXITED CRITICAL REGION********\n");

        // unblock SIGCHLD
        sigprocmask(SIG_UNBLOCK, &sigset, NULL);
        wait(&pid);
    }

    return 0;
}

void chldhandler(int sig) {
    printf("Received SIGCHLD\n");
}
