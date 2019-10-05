#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/types.h>

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
char* line; // dynamically allocated in read_line()
char** tokens; // dynamically allocated in parse_line()
void read_line(); // read into line buffer
void parse_line(); // parse arguments with delimiters
int next_token_length(int); // helper function for parse_line()
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
        .sa_handler = &sigint_handler,
        .sa_flags = 0
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
    int position = 0, length, n = 0, i = 0;
    char *token;
    if (line == NULL) {
        return;
    }
    while ((length = next_token_length(position)) != 0) {
        if (!isspace(line[position])) {
            n++;
        }
        position += length;
    }
    tokens = malloc(sizeof(char*) * (n + 1));
    position = 0;
    while ((length = next_token_length(position)) != 0) {
        if (!isspace(line[position])) {
            token = malloc(sizeof(char) * (length + 1));
            strncpy(token, &line[position], length);
            token[length] = '\0';
            tokens[i++] = token;
        }
        position += length;
    }
    tokens[n] = NULL;
}

int next_token_length(int position) {
    int length = 0;
    if (line[position] == '\0') {
        length = 0;
    } else if (strchr(DELIMITERS, line[position]) != NULL) {
        length = 1;
    } else {
        while (line[position] != '\0' && strchr(DELIMITERS, line[position]) == NULL) {
            position++;
            length++;
        }
    }
    return length;
}

void eval() {
    builtin_func func = get_builtin();
    if (func) {
        func();
        return;
    }

    launch_process();
}

void launch_process() {
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
