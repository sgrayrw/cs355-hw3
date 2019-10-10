#ifndef HW3_MYSH_H
#define HW3_MYSH_H

#include "job.h"
#include "builtin.h"
#include "sighand.h"

#include <ctype.h>
#include <sys/types.h>
#include <errno.h>
#include <stdbool.h>
#include <sys/wait.h>
#include <termios.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#define DEBUG 1
#ifdef DEBUG
# define DEBUG_PRINT(msg) printf msg
#else
# define DEBUG_PRINT(msg) do {} while (0)
#endif

// global vars
char *line; // dynamically allocated in read_line()
char **tokens, **args; // dynamically allocated in parse_line()
int argc, tokens_len;
struct Node* jobs;
int jobcnt;
struct termios mysh_tc;

// main loop
void read_line(); // read into line buffer
void parse_line(); // parse arguments with delimiters
int next_token_length(int); // helper function for parse_line()
void eval(); // evaluate tokens and call builtin/exec
void launch_process(bool);
void free_tokens();

#endif
