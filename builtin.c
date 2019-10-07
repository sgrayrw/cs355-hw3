#include "builtin.h"
struct Node* currentjobs = jobs;
char** currenttokens = tokens;
int length = argc;

void my_jobs(){

    struct Job* currentjob;
    int jobidl;
    enum Status currentstatus;
    char* statusstr;
    char* argument;

    while(current){
        currentjob = currentjobs->job;
        jobid = currentjob->jid;
        currentstatus = currentjob -> status;
        if (currentstatus == Status.Running){
            statusstr = "Running";
        }else{
            statusstr = "Suspended";
        }
        argument = currentjob -> args;
        printf("[%d]   %s    %s\n",jobid,statusstr,arguement);
        currentjobs = currentjobs->next;
    }
}

void my_exit(){
    free_ult();
    printf("Goodbye~");
    exit(EXIT_SUCCESS);
}

void my_kill(){
    int CURRENTSIG=SIGTERM;
    struct Job* currentjob;
    if (strcmp(tokens[length-1],"-9")==0){
        CURRENTSIG = SIGKILL;
    }
    for (int i = 1; i<length-1;i++){
        int jobid = atoi(*currenttokens[i]+1);
        if(get_job(jobid)==NULL){
            printf("Invalid Job ID\n")
        }else{
            currentjob = get_job(jobid);
        }
        int currentpid = currentjob->pid;
        kill(currentpid,CURRENTSIG);
    }
}

void my_fg(){
    int jobid = atoi(*currenttokens[1]+1);
    if(get_job(jobid)==NULL){
        printf("Invalid Job ID\n");
    }else{
        struct Job* currentjob = get_job(jobid);
    };
    int currentpid = currentjob->pid;
    struct termios* currenttermios = currentjob->tcattr;
    int statusnum;
    int* status = &statusnum;
    waitpid(currentpid,status,0);
    remove_job(currentpid);
    tcsetattr(stdin,currenttermios);
    tcsetpgrp(stdin,currentpid);
}

void my_bg(){
    struct Job* currentjob;
    for (int i = 1; i<length;i++){
        int jobid = atoi(*currenttokens[i]+1);
        if(get_job(jobid)==NULL){
            printf("Invalid Job ID\n")
        }else{
            currentjob = get_job(jobid);
        }
        int currentpid = currentjob->pid;
        kill(currentpid,CURRENTCONT);
    }

}