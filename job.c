#include "job.h"

void add_job(pid_t pid, Status status, char* args, struct termios* tcattr) {
    struct Job* job = malloc(sizeof(struct Job));
    if (jobs == NULL) {
        job->jid = 1;
    } else {
        job->jid = jobs->job->jid + 1;
    }
    job->pid = pid;
    job->status = status;
    job->args = malloc(strlen(args) + 1);
    strcpy(job->args, args);
    job->tcattr = malloc(sizeof(struct termios));
    memcpy(job->tcattr, tcattr, sizeof(struct termios));

    struct Node* node = malloc(sizeof(struct Node));
    node->job = job;

    // add to head
    if (jobs == NULL) {
        jobs = node;
        jobs->next = NULL;
        jobs->prev = NULL;
    } else {
        jobs->prev = node;
        node->next = jobs;
        jobs = node;
    }
}
