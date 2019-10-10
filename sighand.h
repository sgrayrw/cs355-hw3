#ifndef HW3_SIGHAND_H
#define HW3_SIGHAND_H

#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>

#define SIG_MIN 1

extern struct termios mysh_tc;

// sighandlers
void initialize_handlers(); // register for signal handlers using sigaction(), initialize sigset for SIGCHLD to protect critical section on job list
void sigint_handler(int); // for ctrl-c
void sigchld_handler(int, siginfo_t *, void *); // for child status changes
void sigusr1_handler(int, siginfo_t *, void *);

#endif
