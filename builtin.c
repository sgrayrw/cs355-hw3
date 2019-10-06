#include "builtin.h"
extern struct Node* currentjobs = jobs;
extern char** currenttokens = tokens;
extern int length = argc;

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
        printf("[%d]   %s    %s",jobid,statusstr,arguement);
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
    if (strcmp(tokens[length-1],"-9")==0){
        CURRENTSIG = SIGKILL;
    }
}
