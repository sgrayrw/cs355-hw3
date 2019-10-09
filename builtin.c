#include "builtin.h"
struct Node* currentjobs = jobs;
char** currenttokens = tokens;
int length = argc;


int builtin(){
    int response = 0;
    if (length == 1 && strcmp(tokens[0],"jobs")==0){
        my_jobs();
        return 1;
    } else if (length == 1 && strcmp(tokens[0],"exit")==0){
        my_exit();
        return 1;
    } else if (length > 1) {
        for (int i = 1; i < length; i++) {
            if (tokens[i][0] != '%' || atoi(tokens[i] + 1) == 0) {
                return response;
            }
        }

        if (strcmp(tokens[0],"kill")==0){
            my_kill();
            return 1;
        } else if (strcmp(tokens[0],"bg")==0){
            my_bg();
            return 1;
        } else if (strcmp(tokens[0],"fg")==0) {
            my_bg();
            return 1;
        }
    }
    return response;
}

void my_jobs(){

    struct Job* currentjob;
    int jobidl;
    enum Status currentstatus;
    char* statusstr;
    char* argument;

    while(currentjob){
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
    free_list();
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
        int jobid = atoi(currenttokens[i]+1);
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
    int jobid = atoi(currenttokens[1]+1);
    if(get_job(jobid)==NULL){
        printf("Invalid Job ID\n");
    }else{
        struct Job* currentjob = get_job(jobid);
    };
    int currentpid = currentjob->pid;
    struct termios* currenttermios = currentjob->tcattr;
    int statusnum;
    int* status = &statusnum;
    tcsetattr(STDIN_FILENO,TCSADRAIN,currenttermios);
    tcsetpgrp(STDIN_FILENO,currentpid);
    kill(currentpid,SIGCONT);
    waitpid(currentpid,status,WUNTRACED);
}

void my_bg(){
    struct Job* currentjob;
    int jobid;
    for (int i = 1; i<length;i++){
        jobid = atoi(currenttokens[i]+1);
        if(get_job(jobid)==NULL){
            printf("Invalid Job ID\n")
        }else{
            currentjob = get_job(jobid);
        }
        int currentpid = currentjob->pid;
        kill(currentpid,SIGCONT);
    }
}