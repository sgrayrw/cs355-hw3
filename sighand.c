#include "sighand.h"
#include "job.h"
#include "mysh.h"

void initialize_handlers() {
    struct sigaction sigint_action = {
        .sa_handler = &sigint_handler,
        .sa_flags = 0
    };
    struct sigaction sigchld_action = {
        .sa_sigaction = &sigchld_handler,
        .sa_flags = SA_RESTART | SA_SIGINFO
    };

    sigemptyset(&sigint_action.sa_mask);
    sigaction(SIGINT, &sigint_action, NULL);

    sigemptyset(&sigchld_action.sa_mask);
    sigaction(SIGCHLD, &sigchld_action, NULL);

    signal(SIGTERM, SIG_IGN);
    signal(SIGTSTP, SIG_IGN);
    signal(SIGQUIT, SIG_IGN);
    signal(SIGTTIN, SIG_IGN);
    signal(SIGTTOU, SIG_IGN);
}

void sigint_handler(int sig) {
    printf("\n");
}

void sigchld_handler(int sig, siginfo_t *info, void *ucontext) {
    struct termios child_tc;
    pid_t child = info->si_pid;
    int status;
    switch (info->si_code) {
        case CLD_EXITED: case CLD_KILLED: case CLD_DUMPED:
            waitpid(info->si_pid, &status, 0);
            remove_job(child);
        case CLD_STOPPED:
            if (tcgetpgrp(STDIN_FILENO) == getpgid(child)) {
                tcgetattr(STDIN_FILENO, &child_tc);
                tcsetpgrp(STDIN_FILENO, getpgid(STDIN_FILENO));
                tcsetattr(STDIN_FILENO, TCSADRAIN, &mysh_tc);
                change_job_status(child, Suspended, &child_tc);
            } else {
                change_job_status(child, Suspended, NULL);
            }
        case CLD_CONTINUED:
            change_job_status(child, Running, NULL);
        default:
            ; //nothing
    }
}
