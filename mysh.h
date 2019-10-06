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

// global vars
char* line; // dynamically allocated in read_line()
extern char** tokens; // dynamically allocated in parse_line()
extern int argc;
extern struct Node *jobs;

// main loop
void read_line(); // read into line buffer
void parse_line(); // parse arguments with delimiters
int next_token_length(int); // helper function for parse_line()
void eval(); // evaluate tokens and call builtin/exec
void free_memory(); // free allocated memory
bool launch_in_background(); // check if line ends with `&` (background job), AND remove if present


#endif
