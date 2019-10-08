#include "sighand.h"
#include "job.h"

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
    pid_t child = info->si_pid;
    int status;
    switch (info->si_code) {
        case CLD_EXITED: case CLD_KILLED: case CLD_DUMPED:
            remove_job(child);
            waitpid(info->si_pid, &status, 0);
        case CLD_STOPPED:
            if (tcgetpgrp() = getpgid(child)) {
                add_job(child, Suspended, tokens, )
            } else {
                change_job_status(child, Suspended);
            }
        case CLD_CONTINUED:
            change_job_status(child, Running);
        default:
            ; //nothing
    }
}
