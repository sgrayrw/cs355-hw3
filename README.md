## Basic structure

Our `mysh` is a simple shell supporting executables, several
built-in commands, and foreground/background job control. Under the hood,
we divided the source of the shell into different parts based on 
its functionalities: shell lifecycle (main loop), built-in commands, signal handling, 
and job control.

The main lifecycle consists of several stages: reading from `stdin`, 
parsing the input into arguments, executing relevant commands, and 
performing necessary memory clean-up.

Built-in commands are implemented in a separate section from and called before 
regular executables that are executed by forking off child processes. A
list of supported built-in commandes are listed below.

The signal handling section is responsible for masking/registering for
necessary signals to ensure that the shell functions properly. It also listens
to the status of child processes.

The job control is supported by two layers of doubly linked lists: one
that records each non-built-in job executed (both foreground and background), while
the other keeps track of the order in which job status changes in the background to enable
the relative ordering of `fg` and `bg` commands.

Some other various error handling strategies are implemented to maximize
the reliability of the shell.

## Supported features
**Fully implemented**
- built-in
    - `exit`, `job`
    - `kill [%<#>]`: sends `SIGTERM` with no flag (which we found that 
    does not terminate suspended processes); sends `SIGKILL` with flag `-9`
    - `fg [%<#>]`, `bg [%<#>]`
- handle multiple commands with `;`
- retrieve and change job status asynchronously and print status
of changed jobs synchronounsly before next prompt
  

**Not implemented**
- pipes and redirections

## Testing the shell

We tested our shell with respect to the following aspects:

**Signal handling**

- we had another process sending various signals that would terminate/hang
a process to the shell. The shell successfully handles all signals
that could be handled without affecting behavior of the child processes.
- We tested concurrency handling abilities of the signal handlers by 
firing off hundreds of child processes that would terminate at the same
time. All `SIGCHLD` signals were properly handled and there was no lost.

**Concurrency**

In addition to the concurrency handling of signal handlers,
we tested cases where there are jobs created at the same time
when jobs are terminated/suspended. The job list were performing properly.

**Built-in commands**

We tested different flags/options regarding the built-in commands we support.

- `kill [%<#>]`: properly handles multiple targets within one line;
`-9` flag works properly
- `fg [%<#>]`, `bg [%<#>]`: successfully operates on the last
job backgrounded/suspended with no flag or `%` without job id

**Random inputs**

We tested our shell and built-in commands against random inputs
including nonexistent commands, correct input with random whitespaces, 
correct command with nonexistent/wrong flag/option. The shell responds to
them properly with relevant error messages.
