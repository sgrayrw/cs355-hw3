# run with `$ bash signals.sh` in mysh
# test signal handling

myshpid=`ps -aux | grep ./mysh | head -n 1 | awk '{print $2}'`
echo "sending SIGTERM"
kill -SIGTERM $myshpid
echo "sending SIGTSTP"
kill -SIGTSTP $myshpid
echo "sending SIGQUIT"
kill -SIGQUIT $myshpid
echo "sending SIGTTIN"
kill -SIGTTIN $myshpid
echo "sending SIGTTOU"
kill -SIGTTOU $myshpid
echo "sending SIGINT (expected to print a new line)"
kill -SIGINT $myshpid
echo "mysh survived signals!!!"
