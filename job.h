#ifndef HW3_JOB_H
#define HW3_JOB_H

#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>

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
};

extern struct Node* jobs;

struct Job* get_job(int jid);
void add_job(pid_t pid, Status status, char* args, struct termios* tcattr);
int remove_job(pid_t pid);
void change_job_status(pid_t pid, Status status);
void free_node(struct Node* node);
void free_list();

#endif
