# run with `$source jobs.sh` in mysh
# test job control

echo "cat - &"
sleep 1
cat - &
sleep 1
echo "sleep 50 &"
sleep 1
sleep 30 &
sleep 1
echo "sleep 3 &"
sleep 1
sleep 5 &
sleep 1
echo "jobs"
sleep 1
jobs
sleep 1
echo "kill %1"
sleep 1
kill %1
sleep 1
echo "jobs"
sleep 1
jobs
sleep 1
echo "kill %2"
sleep 1
kill %2
sleep 1
echo "jobs"
sleep 1
jobs
sleep 1

echo "bash loop.sh &"
sleep 1
bash loop.sh &
sleep 5
echo "jobs"
sleep 1
jobs
sleep 1
echo "fg"
echo "need ctrl-z now"
fg
sleep 3
echo "bg"
sleep 1
echo "now running in background as if started with \`&\`"
bg
echo "kill %1"
sleep 1
kill 1
sleep 1
echo "jobs"
sleep 1
jobs
sleep 1
echo "mysh survived jobs!!!"