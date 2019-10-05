#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define DELIMITERS " \f\n\r\t\v"

// builtin functions
typedef builtin_func void (builtin_func)(void) // type for builtin functions
builtin_func get_builtin(); // return the appropriate built-in func based on `tokens`, NULL if no match
void my_exit();
void jobs(); // list all background jobs
void kill(); // kill a background job
void fg(); // put a specific job to the foreground
void bg(); // continue a specific job in the background

// job related
struct Node {
    struct Job job;
    Node next;
};

struct Job {
    int jid; // job id
    pid_t pid;
    enum Status {Running, Suspended};
    char* args; // args that started the job
    // terminal modes
};

Node jobs; // head of linked list
void add_job(pid_t pid, enum Status status, char* args); // add job to linked list
void remove_job(pid_t pid); // remove job from linked list

// sighandlers
void initialize_handlers(); // register for signal handlers using sigaction(), initialize sigset for SIGCHLD to protect critical section on job list
sigset_t sigset; // sigset to block to protect critical section
void sigint_handler(int); // for ctrl-c
void chldhandler(int, siginfo_t*, void*); // for child suspension and termination
// other signals that may cause the shell to terminate

// main loop related
char* line // dynamically allocated in read_line()
char** tokens // dynamically allocated in parse_line()
void read_line(); // read into line buffer
void parse_line(); // parse arguments with delimiters
char *next_token(int); // helper function for parse_line()
void eval(); // evaluate tokens and call builtin/exec
void free_storage(); // free allocated memory
bool is_background(); // check if line ends with `&` (background job), AND remove if present

// *********************************** data structure/function defs and global vars above ***********************************

int main() {
    initialize_handlers(); // register for signal handlers

    while (1) {
        read_line(); // read into line buffer
        parse_line(); // parse arguments
        eval(); // evaluate arguments
        free_storage(); // free spaces
    }
    return 0;
}

void initialize_handlers() {
    struct sigaction sigint_action = {
        .sa_handler = &inthandler;
        .sa_flags = 0;
    };
    sigemptyset(&sigint_action.sa_mask);
    sigaction(SIGINT, &sigint_action, NULL);
    
    signal(SIGTERM, SIG_IGN);
    signal(SIGTSTP, SIG_IGN);
    signal(SIGQUIT, SIG_IGN);
    signal(SIGTTIN, SIG_IGN);
    signal(SIGTTOU, SIG_IGN);
}
    
void sigint_handler(int sig) {
    printf("\n");
}

void read_line() {
    size_t n;
    printf("[mysh]$ ");
    if (getline(&line, &n, stdin) == -1) {
        my_exit();
    }
}

void parse_line() {
    int position, n, i;
    if (line == NULL) {
        return;
    }
    while (next_token(position) != NULL) {
        n++;
    }
    tokens = malloc(sizeof(char*) * (n + 1));
    while ((tokens[i++] = next_token(position)) != NULL);
}

char *next_token(int position) {
    char *token;
    int length, i;
    if (line[position] == NULL) {
        return NULL;
    } else if (strchr(DELIMITERS, line[position]) != NULL) {
        length = 1;
    } else {
        i = position;
        while (line[i] != NULL && strchr(DELIMITERS, line[i]) == NULL) {
            length++;
            i++;
        }
    }
    token = malloc(sizeof(char) * (length + 1));
    for (i = 0; i < length; i++, position++) {
        token[i] = line[position];
    }
    token[i] = NULL;
    return token;
}

void eval() {
    builtin_func func = get_builtin();
    if (func) {
        func();
        return;
    }

    // exec call
    pid_t pid = fork();
    if (pid == 0) { // child
        // need to set signal handlers back (to default behavior)
        setpgrp();
        exec();
    } else { // parent
        if (is_background()) {
            sigprocmask(BLOCK, &sigset, NULL); // mask SIGCHLD to protect CS
            add_job(pid);
            sigprocmask(UNBLOCK, &sigset, NULL);
            return;
        } else {
            tcsetpgrp(0, pid);
            waitpid(pid, &status);
        }
    }
}
