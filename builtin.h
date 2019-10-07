#ifndef HW3_BUILTIN_H
#define HW3_BUILTIN_H

#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include "job.h"
#include "mysh.h"

// builtin functions
typedef builtin_func void (builtin_func)(void) // type for builtin functions
builtin_func get_builtin(char** tokens); // return the appropriate built-in func based on `tokens`, NULL if no match
void my_exit();
void my_jobs(); // list all background jobs
void my_kill(); // kill a background job
void my_fg(); // put a specific job to the foreground
void my_bg(); // continue a specific job in the background

#endif