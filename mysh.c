// builtin functions
typedef builtin_func ... // type for builtin functions
builtin_func get_builtin(); // return the appropriate built-in func based on `tokens`, NULL if no match
void my_exit();
void jobs(); // list all background jobs
void kill(); // kill a background job
void fg(); // put a specific job to the foreground
void bg(); // continue a specific job in the background

// job related
struct Node {
  struct Job;
  Node next;
};

struct Job {
  int jid; // job id
  pid_t pid;
  enum Status {Running, Suspended};
  char* args; // args that started the job
};

Node jobs; // head of linked list
void add_job(pid_t pid, enum Status status, char* args); // add job to linked list
void remove_job(pid_t pid); // remove job from linked list

// sighandlers
void initialize_handlers(); // register for signal handlers using sigaction(), initialize sigset for SIGCHLD to protect critical section on job list
sigset_t sigset; // sigset to block to protect critical section
void* inthandler(int, siginfo_t*, void*); // for ctrl-c
void* tstphandler(int, siginfo_t*, void*); // for ctrl-z
void chldhandler(int, siginfo_t*, void*); // for child suspension and termination

// main loop related
char* line // dynamically allocated in read_line()
char** tokens // dynamically allocated in parse_line()
void read_line(); // read into line buffer
void parse_line(); // parse arguments with delimiters
void eval(); // evaluate tokens and call builtin/exec
void free_storage(); // free allocated memory
bool is_background(); // check if line ends with `&` (background job), AND remove if present

// *********************************** data structure/function defs and global vars above ***********************************

int main() {
  initialize_handlers(); // register for signal handlers
  
  while (1) {
    read_line(); // read into line buffer
    parse_line(); // parse arguments
    eval(); // evaluate arguments
    free_storage(); // free spaces
  }
  return 0;
}

void eval(){
  builtin_func func = get_builtin();
  if (func) {
    func();
    return;
  }
  
  // exec call
  pid_t pid = fork();
  if (pid == 0) { // child
    setpgrp();
    exec();
  } else { // parent
    if (is_background()) {
      sigprocmask(BLOCK, &sigset, NULL); // mask SIGCHLD to protect CS
      add_job(pid);
      sigprocmask(UNBLOCK, &sigset, NULL);
      return;
    } else {
      tcsetpgrp(0, pid);
      waitpid(pid, &status);
    }
  }
  
}

