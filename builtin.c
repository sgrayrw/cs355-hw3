#include "builtin.h"
#include "job.h"
#include "mysh.h"

struct Node* bg_jobs;
struct Job* lastjob;
char** currenttokens;
int length;

void getlastjob();

int builtin(char** neededtokens, int argclength){ 
    currenttokens = neededtokens;
    length = argclength;
    if (length == 1 && strcmp(currenttokens[0],"jobs")==0){
        my_jobs();
        return true;
    } else if (length == 1 && strcmp(currenttokens[0],"exit")==0) {
        my_exit();
        return true;
    } else if (strcmp(currenttokens[0],"kill")==0 && length > 1) {
        int argc_length = length;
        if (strcmp(currenttokens[length - 1], "-9") == 0 && length > 2) {
            argc_length--;
        }
        for (int i = 1; i < argc_length-1; i++) {
            if (currenttokens[i][0] != '%' || (strlen(currenttokens[i]) > 1 && atoi(currenttokens[i] + 1) == 0)) {
                return false;
            }
        }
        my_kill();
        return true;
    } else if (length > 1){
        for (int i = 1; i < length-1; i++) {
            if (currenttokens[i][0] != '%' || (strlen(currenttokens[i]) > 1 && atoi(currenttokens[i] + 1) == 0)) {
                return false;
            }
        }
        if (strcmp(currenttokens[0],"bg")==0){
            my_bg();
            return true;
        } else if (strcmp(currenttokens[0],"fg")==0) {
            my_fg();
            return true;
        }
    }
    return false;
}

void my_jobs(){
    bg_jobs = jobs;
    struct Node* currentjobs = bg_jobs;
    struct Job* currentjob;
    int jobid;
    Status currentstatus;
    char* statusstr;
    char* arguement;

    while(currentjobs){
        currentjob = currentjobs->job;
        jobid = currentjob->jid;
        currentstatus = currentjob -> status;
        if (currentstatus == Running){
            statusstr = "Running";
        }else{
            statusstr = "Suspended";
        }
        arguement = currentjob -> args;
        printf("[%d]   %s    %s\n",jobid,statusstr,arguement);
        currentjobs = currentjobs->next;
    }
}

void my_exit(){
    free_list();
    printf("Goodbye~");
    exit(EXIT_SUCCESS);
}

void my_kill(){
    int lastjob_done = false;
    int CURRENTSIG=SIGTERM;
    struct Job* currentjob;
    int currentpid;
    if (strcmp(currenttokens[length-1],"-9")==0){
        CURRENTSIG = SIGKILL;
    }
    for (int i = 1; i<length-1;i++){
        if (strcmp(currenttokens[i],"%")==0){
            if (lastjob_done == true){
                continue;
            }
            lastjob_done = true;
            getlastjob();
            currentpid = lastjob->pid;
        }else{
            int jobid = atoi(currenttokens[i]+1);
            if(get_job(jobid)==NULL){
                printf("Invalid Job ID\n");
                return;
            }else{
                currentjob = get_job(jobid);
            }
            currentpid = currentjob->pid;
        }

        kill(currentpid,CURRENTSIG);
    }
}

void my_fg(){
    int currentpid;
    struct Job *currentjob;
    if (strcmp(currenttokens[1],"%")==0){
        getlastjob();
        currentjob = lastjob;
    }else {
        int jobid = atoi(currenttokens[1] + 1);
        if (get_job(jobid) == NULL) {
            printf("Invalid Job ID\n");
            return;
        } else {
            currentjob = get_job(jobid);
        }
    }
    currentpid = currentjob->pid;
    struct termios* currenttermios = currentjob->tcattr;
    int statusnum;
    int* status = &statusnum;
    tcsetattr(STDIN_FILENO,TCSADRAIN,currenttermios);
    tcsetpgrp(STDIN_FILENO,currentpid);
    kill(currentpid,SIGCONT);
    waitpid(currentpid,status,WUNTRACED);
}

void my_bg(){
    int lastjob_done = false;
    struct Job* currentjob;
    int jobid;
    int currentpid;
    for (int i = 1; i<length;i++){
        if (strcmp(currenttokens[i],"%")==0){
            if (lastjob_done == true){
                continue;
            }
            lastjob_done = true;
            getlastjob();
            currentpid = lastjob->pid;
        }else {
            jobid = atoi(currenttokens[i] + 1);
            if (get_job(jobid) == NULL) {
                printf("Invalid Job ID\n");
                return;
            } else {
                currentjob = get_job(jobid);
            }
            currentpid = currentjob->pid;
        }
        kill(currentpid,SIGCONT);
    }
}

void getlastjob(){
    bg_jobs = jobs;
    struct Node* currentjobs = bg_jobs;
    while (currentjobs->next){
        currentjobs = currentjobs->next;
    }
    lastjob = currentjobs->job;
}