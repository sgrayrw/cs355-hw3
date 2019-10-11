#include "job.h"
#include "mysh.h"

struct Job* get_job_jid(int jid) {
    for (struct Node* node = jobs; node; node = node->next) {
        if (node->job->jid == jid) {
            return node->job;
        }
    }
    return NULL;
}

struct Job* get_job_pid(pid_t pid) {
    for (struct Node* node = jobs; node; node = node->next) {
        if (node->job->pid == pid) {
            return node->job;
        }
    }
    return NULL;
}

int add_job(pid_t pid, Status status, int _argc, char** args, struct termios* tcattr) {
    struct Job* job = malloc(sizeof(struct Job));
    if (jobs == NULL) {
        job->jid = 1;
    } else {
        job->jid = jobs->job->jid + 1;
    }
    job->pid = pid;
    job->status = status;
    job->argc = _argc;
    job->args = malloc(sizeof(char*) * _argc);
    for (int i = 0; i < _argc; ++i) {
        job->args[i] = malloc(strlen(args[i]) + 1);
        strcpy(job->args[i], args[i]);
    }
    job->tcattr = malloc(sizeof(struct termios));
    memcpy(job->tcattr, tcattr, sizeof(struct termios));
    job->status_changed = false;

    struct Node* node = malloc(sizeof(struct Node));
    node->job = job;

    // add to head (CS, masking SIGCHLD)
    sigset_t sigset;
    sigemptyset(&sigset);
    sigaddset(&sigset, SIGCHLD);
    sigprocmask(SIG_BLOCK, &sigset, NULL);
    if (jobs == NULL) {
        jobs = node;
        jobs->next = jobs;
        jobs->prev = jobs;
    } else {
        node->next = jobs;
        node->prev = jobs->prev;
        jobs->prev->next = node;
        jobs->prev = node;
    }
    sigprocmask(SIG_UNBLOCK, &sigset, NULL);
    return job->jid;
}

void remove_job(struct Node* node) {
    if (node->job->jid == jobs->job->jid) {
        free_node(jobs);
        jobs = NULL;
    } else {
        node->next->prev = node->prev;
        node->prev->next = node->next;
        free_node(node);
    }
}

void change_job_status(pid_t pid, Status status, struct termios* tcattr) {
    for (struct Node* node = jobs; node; node = node->next) {
        if (node->job->pid == pid) {
            node->job->status = status;
            if (tcattr) {
                free(node->job->tcattr);
                node->job->tcattr = malloc(sizeof(struct termios));
                memcpy(node->job->tcattr, tcattr, sizeof(struct termios));
            }
            node->job->status_changed = true;
        }
    }
}

void unchange_status(pid_t pid) {
    for (struct Node* node = jobs; node; node = node->next) {
        if (node->job->pid == pid) {
            node->job->status_changed = false;
        }
    }
}

void process_changed_jobs(bool print) {
    for (struct Node* node = jobs; node && node->job->status_changed; node = node->next) {
        if (print) {
            print_job(node->job);
        }
        if (node->job->status == Done || node->job->status == Terminated) {
            remove_job(node);
        } else {
            unchange_status(node->job->pid);
        }
    }
}

void print_job(struct Job* job) {
    char* statusstr;
    switch (job->status) {
        case Running:
            statusstr = "Running";
            break;
        case Suspended:
            statusstr = "Suspended";
            break;
        case Done:
            statusstr = "Done";
            break;
        case Terminated:
            statusstr = "Terminated";
            break;
    }
    printf("[%d]\t%s\t", job->jid, statusstr);
    for (int i = 0; i < job->argc; ++i){
        printf("%s ", job->args[i]);
    }
    printf("\n");
}

void free_node(struct Node* node) {
    free(node->job->tcattr);
    node->job->tcattr = NULL;
    for (int i = 0; i < node->job->argc; ++i) {
        free(node->job->args[i]);
        node->job->args[i] = NULL;
    }
    free(node->job->args);
    node->job->args = NULL;
    free(node->job);
    node->job = NULL;
    free(node);
    node = NULL;
}

void free_list() {
    struct Node* node = jobs;
    while (node) {
        jobs = jobs->next;
        free_node(node);
        node = jobs;
    }
}
