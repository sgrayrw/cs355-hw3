#include "mysh.h"

#define DELIMITERS ";& \f\n\r\t\v"

int main() {
    jobs = NULL;
    initialize_handlers(); // register for signal handlers
    tcgetattr(STDIN_FILENO, &mysh_tc);

    while (true) {
        read_line(); // read into line buffer
        parse_line(); // parse arguments
        eval(); // evaluate arguments
    }
    free_args();
    free_tokens();
}

void read_line() {
    size_t n = 0;
    printf("mysh ❯ ");
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
    tokens_len = n;
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
    int i, start_pos = 0, end_pos;
    bool background, is_semicolon;
    for (i = 0; i < tokens_len; i++) {
        is_semicolon = strcmp(tokens[i], ";") == 0;
        if (is_semicolon || i == tokens_len - 1) {
            if (i == tokens_len - 1 && !is_semicolon || i > start_pos) {
                if (i == tokens_len - 1 && !is_semicolon){
                    end_pos = i;
                } else {
                    end_pos = i - 1;
                }
                argc = end_pos - start_pos + 1;
                args = malloc(sizeof(char *) * (argc + 1));
                memcpy(args, &tokens[start_pos], sizeof(char *) * argc);
                args[argc + 1] = NULL;
                if (strcmp(args[argc - 1], "&") == 0) {
                    argc--;
                    args[argc] = NULL;
                    background = true;
                } else {
                    background = false;
                }
                launch_process(background);
            }
            start_pos = i + 1;
        }
    }
}

void launch_process(bool background) {
    int i, status;
    pid_t pid;
    struct termios tc_attr;

    if (builtin(args, argc) == true) {
        return;
    }

    pid = fork();

    if (pid == 0) { // child
        for (i = SIG_MIN; i < NSIG; i++) {
            signal(i, SIG_DFL);
        }
        setpgrp();
        if (execvp(args[0], args) == -1) {
            if (errno == ENOENT) {
                fprintf(stderr, "No such file or directory.\n");
            } else {
                fprintf(stderr, "%s: command not found.\n", tokens[0]);
            }
            exit(EXIT_FAILURE);
        }
    } else if (pid > 0) { // parent
        setpgid(pid, pid);
        add_job(pid, Running, argc, args, &mysh_tc);
        if (!background) {
            tcsetpgrp(STDIN_FILENO, pid);
            waitpid(pid, &status, WUNTRACED);
            tcsetpgrp(STDIN_FILENO, getpgrp());
            tcsetattr(STDIN_FILENO, TCSADRAIN, &mysh_tc);
        }
    } else {
        fprintf(stderr, "Error forking a process.\n");
    }
}

void free_tokens() {
    int i;
    if (tokens != NULL){
        for (i = 0; i < tokens_len; i++) {
            if (tokens[i] != NULL) {
                free(tokens[i]);
                tokens[i] = NULL;
            }
        }
        free(tokens);
        tokens = NULL;
    }
    free(line);
}

void free_args() {
    int i;
    if (args != NULL) {
        for (i = 0; i < argc; i++) {
            if (args[i] != NULL) {
                free(args[i]);
                args[i] = NULL;
            }
        }
        free(args);
        args = NULL;
    }
}
