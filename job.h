#ifndef HW3_JOB_H
#define HW3_JOB_H

#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>

extern struct Node* jobs;

typedef enum {
    Running,
    Suspended
} Status;

struct Job {
    int jid; // job id
    pid_t pid;
    Status status;
    char* args; // args that started the job
    struct termios* tcattr;
};

// job related
struct Node {
    struct Job* job;
    struct Node* next;
    struct Node* prev;
};

void add_job(pid_t pid, Status status, char* args, struct termios* tcattr);
void remove_job(pid_t pid);

#endif