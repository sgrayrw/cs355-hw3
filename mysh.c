#include "mysh.h"

#define DELIMITERS "& \f\n\r\t\v"

int main() {
    initialize_handlers(); // register for signal handlers
    tcgetattr(STDIN_FILENO, &mysh_tc);

    while (true) {
        tcsetattr(STDIN_FILENO, TCSADRAIN, &mysh_tc);
        read_line(); // read into line buffer
        parse_line(); // parse arguments
        eval(); // evaluate arguments
        free_memory(); // free spaces
    }
}

void read_line() {
    size_t n = 0;
    printf("[mysh]$ ");
    if (getline(&line, &n, stdin) == -1) {
        if (feof(stdin)) {
            my_exit();
        } else {
            clearerr(stdin);
        }
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
    argc = n;
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
    // call builtin OR exec child
//    builtin();
    launch_process();
}

void launch_process() {
    int i;
    pid_t pid = fork();
    struct termios tc_attr;
    bool background = launch_in_background();
    if (pid == 0) { // child
        for (i = SIG_MIN; i < NSIG; i++) {
            signal(i, SIG_DFL);
        }
        setpgrp();
        if (execvp(tokens[0], tokens) == -1) {
            if (errno == ENOENT) {
                fprintf(stderr, "No such file or directory.\n");
            } else {
                fprintf(stderr, "%s: command not found.\n", tokens[0]);
            }
            free_memory();
            exit(EXIT_FAILURE);
        }
    } else if (pid > 0) { // parent
        setpgid(pid, pid);
        if (background) {
            add_job(pid, Running, line, &mysh_tc);
        } else {
            tcsetpgrp(STDIN_FILENO, pid);
            int status;
            waitpid(pid, &status, WUNTRACED);
        }
    } else {
        fprintf(stderr, "Error forking a process.\n");
    }
}

bool launch_in_background() {
    if (strcmp(tokens[argc - 1], "&") == 0) {
        tokens[argc - 1] = realloc(tokens[argc - 1], sizeof(char));
        tokens[argc - 1][0] = '\0';
        free(tokens[argc]);
        return true;
    } else {
        return false;
    }
}
