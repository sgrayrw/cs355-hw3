#include "builtin.h"
#include "job.h"
#include "mysh.h"

struct Node* bg_jobs;
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
    bg_jobs = jobs;
    struct Job* reverse_job[jobcnt];
    struct Node* currentjobs = bg_jobs;
    struct Job* currentjob;
    int jobid;
    int i = 0;
    Status currentstatus;
    char* statusstr;
    char* arguement;
    while(currentjobs) {
        currentjob = currentjobs->job;
        reverse_job[i] = currentjob;
        currentjobs = currentjobs -> next;
        i++;
    }
    for (int i = jobcnt-1; i>=0;i--){
        currentjob = reverse_job[i];
        jobid = currentjob->jid;
        currentstatus = currentjob -> status;
        if (currentstatus == Running){
            statusstr = "Running";
        }else if (currentstatus == Suspended){
            statusstr = "Suspended";
        }else if(currentstatus == Done){
            statusstr = "Done";
        }else if(currentstatus == Terminated){
            statusstr = "Terminated";
        }
        printf("[%d]   %s    ",jobid,statusstr);
        for (int i = 0; i<currentjob->argc; i++){
            arguement = currentjob -> args[i];
            printf("%s ",arguement);
        }
        printf("\n");
    }
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
    bg_jobs = jobs;
    if (jobs == NULL){
        printf("No current job\n");
        return false;
    }
    struct Node* currentjobs = bg_jobs;
    lastjob = currentjobs->job;
    return true;
}
