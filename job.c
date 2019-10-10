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

void add_job(pid_t pid, Status status, int _argc, char** args, struct termios* tcattr) {
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
        jobs->next = NULL;
    } else {
        node->next = jobs;
        jobs = node;
    }
    sigprocmask(SIG_UNBLOCK, &sigset, NULL);
    jobcnt++;
}

int remove_job(pid_t pid) {
    struct Node* cur = jobs, *prev = NULL;

    if (cur && cur->job->pid == pid) {
        jobs = cur->next;
        free_node(cur);
        jobcnt --;
        return 0;
    }

    while (cur && cur->job->pid != pid) {
        prev = cur;
        cur = cur->next;
    }

    if (cur == NULL) {
        return -1;
    } else {
        prev->next = cur->next;
        free_node(cur);
        jobcnt--;
        return 0;
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
    struct Job* reverse_job[jobcnt];
    struct Node* cur = jobs;
    struct Job* curjob;
    int i = 0;
    char* status;
    while(cur) {
        curjob = cur->job;
        reverse_job[i] = curjob;
        cur = cur -> next;
        i++;
    }

    for (int i = jobcnt-1; i>=0; i--){
        curjob = reverse_job[i];
        if (!curjob->status_changed) {
            continue;
        }

        if (print) {
            switch (curjob->status) {
                case Running:
                    status = "Running";
                    break;
                case Suspended:
                    status = "Suspended";
                    break;
                case Done:
                    status = "Done";
                    break;
                case Terminated:
                    status = "Terminated";
                    break;
            }
            printf("[%d]   %s    ", curjob->jid, status);
            for (int i = 0; i < curjob->argc; i++){
                printf("%s ", curjob->args[i]);
            }
            printf("\n");
        }
        if (curjob->status == Done || curjob->status == Terminated) {
            remove_job(curjob->pid);
        } else {
            unchange_status(curjob->pid);
        }
    }
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
