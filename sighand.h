#include <signal.h>
#include <stdlib.h>
#include <stdio.h>

#ifndef HW3_SIGHAND_H
#define HW3_SIGHAND_H

// sighandlers
void initialize_handlers(); // register for signal handlers using sigaction(), initialize sigset for SIGCHLD to protect critical section on job list
void sigint_handler(int); // for ctrl-c
void chldhandler(int, siginfo_t*, void*); // for child suspension and termination
// other signals that may cause the shell to terminate

#endif