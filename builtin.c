#include "builtin.h"
#include "job.h"
#include "mysh.h"

struct Job* lastjob;
char** currenttokens;
int length;

int getlastjob();

int builtin(char** neededtokens, int argclength){
    currenttokens = neededtokens;
    length = argclength;
    if (length > 0 ){
        if (strcmp(currenttokens[0],"jobs")==0){
            my_jobs();
            return true;
        } else if (strcmp(currenttokens[0],"exit")==0) {
            my_exit();
            return true;
        } else if (strcmp(currenttokens[0],"kill")==0) {
            my_kill();
            return true;
        } else if (strcmp(currenttokens[0],"bg")==0) {
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
    struct Node* currentjobs = jobs;
    if(!currentjobs){
        return;
    }
    int startid = currentjobs -> job -> jid;
    while (currentjobs->prev->job->jid != startid){
        print_job(currentjobs->job);
        currentjobs = currentjobs->prev;
    }
    print_job(currentjobs->job);
}

void my_exit(){
    free_list();
    free_tokens();
    printf("Goodbye~\n");
    exit(EXIT_SUCCESS);
}

void my_kill(){
    if (length == 1 || (length == 2 && strcmp(currenttokens[length-1],"-9")==0)){
        printf("not enough arguments\n");
        return;
    }

    int argc_start = 1;
    int lastjob_done = false;
    int CURRENTSIG=SIGTERM;
    struct Job* currentjob;
    int currentpid;
    int jobid;

    if (strcmp(currenttokens[1],"-9")==0){
        CURRENTSIG = SIGKILL;
        argc_start ++;
    }

    for (int i = argc_start; i<length;i++){
        if (currenttokens[i][0] != '%'){
            printf("kill: illegal argument: %s\n",currenttokens[i]);
            return;
        }

        if (strcmp(currenttokens[i],"%")==0){
            if (lastjob_done == true){
                continue;
            }
            lastjob_done = true;
            if(getlastjob()==false){
                return;
            }
            currentpid = lastjob->pid;
        }else{
            if ((jobid = atoi(currenttokens[i]+1))==0){
                printf("kill: illegal pid: %s\n", currenttokens[i]);
                return;
            };
            if(get_job_jid(jobid)==NULL){
                printf("kill: no such job: %s\n", currenttokens[i]);
                return;
            }else{
                currentjob = get_job_jid(jobid);
            }
            currentpid = currentjob->pid;
        }
        if(kill(currentpid,CURRENTSIG)==0){
            printf("Success: %d %d\n", currentpid, CURRENTSIG);
        }else{
            printf("Fail: %d %d\n", currentpid, CURRENTSIG);
        }

    }
}

void my_fg(){
    int currentpid;
    int jobid;
    struct Job *currentjob;
    if (length == 1 || strcmp(currenttokens[1],"%")==0){
        if(getlastjob()==false){
            return;
        }
        currentjob = lastjob;
    } else if (currenttokens[1][0] != '%'){
        printf("fg: illegal argument: %s\n",currenttokens[1]);
        return;
    } else {
        if ((jobid = atoi(currenttokens[1] + 1))==0){
            printf("fg: illegal pid: %s\n", currenttokens[1]);
            return;
        }
        if (get_job_jid(jobid) == NULL) {
            printf("fg: no such job: %s\n", currenttokens[1]);
            return;
        } else {
            currentjob = get_job_jid(jobid);
        }
    }
    currentpid = currentjob->pid;
    struct termios* currenttermios = currentjob->tcattr;
    int statusnum;
    int* status = &statusnum;
    kill(currentpid,SIGCONT);
    waitpid(currentpid,status,WCONTINUED);
    tcsetpgrp(STDIN_FILENO,currentpid);
    tcsetattr(STDIN_FILENO,TCSADRAIN,currenttermios);
    waitpid(currentpid,status,WUNTRACED);
}

void my_bg(){
    int lastjob_done = false;
    struct Job* currentjob;
    int jobid;
    int currentpid;

    if (length == 1){
        if(getlastjob()==false){
            return;
        }
        currentpid = lastjob->pid;
        kill(currentpid,SIGCONT);
    }

    for (int i = 1; i<length;i++){
        if (currenttokens[i][0] != '%') {
            printf("bg: illegal argument: %s\n", currenttokens[i]);
            return;
        }
        if (strcmp(currenttokens[i],"%")==0){
            if (lastjob_done == true){
                continue;
            }
            lastjob_done = true;
            if(getlastjob()==false){
                return;
            }
            currentpid = lastjob->pid;
        }else {
            if((jobid = atoi(currenttokens[i] + 1))==0){
                printf("bg: illegal pid: %s\n", currenttokens[i]);
                return;
            }
            if (get_job_jid(jobid) == NULL) {
                printf("bg: no such job: %s\n", currenttokens[i]);
                return;
            } else {
                currentjob = get_job_jid(jobid);
            }
            currentpid = currentjob->pid;
        }
        kill(currentpid,SIGCONT);
    }
}

int getlastjob(){
    if (jobs == NULL){
        printf("No current job\n");
        return false;
    }
    struct Node* currentjobs = jobs;
    lastjob = currentjobs->job;
    return true;
}
