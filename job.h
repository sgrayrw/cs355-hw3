#ifndef HW3_JOB_H
#define HW3_JOB_H

#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <stdbool.h>

typedef enum {
    Running,
    Suspended,
    Done,
    Terminated
} Status;

struct Job {
    int jid; // job id
    pid_t pid;
    Status status;
    int argc;
    char** args; // args that started the job
    struct termios* tcattr;
    bool status_changed;
};

// job related
struct Node {
    struct Job* job;
    struct Node* next;
};

extern struct Node* jobs;
extern int jobcnt;

struct Job* get_job_jid(int jid);
struct Job* get_job_pid(pid_t pid);
void add_job(pid_t pid, Status status, int argc, char** args, struct termios* tcattr);
int remove_job(pid_t pid);
void change_job_status(pid_t pid, Status status, struct termios* tcattr);
void process_changed_jobs(bool print);
void free_node(struct Node* node);
void free_list();

#endif
