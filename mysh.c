#include "job.h"
#include "builtin.h"
#include "sighand.h"

#include <ctype.h>
#include <sys/types.h>
#include <errno.h>
#include <stdbool.h>
#include <sys/wait.h>

#define DELIMITERS " \f\n\r\t\v"

// global vars
char* line; // dynamically allocated in read_line()
char** tokens; // dynamically allocated in parse_line()

// main loop
void read_line(); // read into line buffer
void parse_line(); // parse arguments with delimiters
int next_token_length(int); // helper function for parse_line()
void eval(); // evaluate tokens and call builtin/exec
void free_memory(); // free allocated memory
bool launch_in_background(); // check if line ends with `&` (background job), AND remove if present


int main() {
    initialize_handlers(); // register for signal handlers

    while (true) {
        read_line(); // read into line buffer
        parse_line(); // parse arguments
        eval(); // evaluate arguments
        free_memory(); // free spaces
    }
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
    int i;
    pid_t pid = fork();
    if (pid == 0) { // child
        for (i = 1; i < 32; i++) {
            signal(i, SIG_DFL);
        }
        setpgrp();
        if (execvp(tokens[0], tokens) == -1) {
            if (errno = ENOENT) {
                fprintf(stderr, "No such file or directory.\n");
            } else {
                fprintf(stderr, "%s: command not found.\n", tokens[0]);
            }
            free_memory();
            exit(EXIT_FAILURE);
        }
    } else if (pid > 0) { // parent
        setpgid(pid, pid);
        if (launch_in_background()) {
            // TODO: mask SIGCHLD to protect CS
            add_job(pid);
        } else {
            tcsetpgrp(stdin, pid);
            waitpid(pid, &status);
        }
    } else {
        fprintf(stderr, "Error forking a process.\n");
    }
}
