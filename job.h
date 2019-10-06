#include <unistd.h>

#ifndef HW3_JOB_H
#define HW3_JOB_H

struct Job {
    int jid; // job id
    pid_t pid;
    enum Status {Running, Suspended};
    char* args; // args that started the job
    // terminal modes
};

// job related
struct Node {
    struct Job job;
    struct Node* next;
};

void add_job(pid_t pid, enum Status status, char* args); // add job to linked list
void remove_job(pid_t pid); // remove job from linked list

#endif