# run with `$ source lots_of_children.sh` in mysh
# create lots of processes in the background to test concurrency handling

echo "firing off 100 child processes"

for (( i=0; i < 100; ++i ))
do
    echo "child $i sleeping"
    sleep 5 &
done

echo "jobs"
sleep 1
jobs
sleep 1
echo "mysh survived 100 child processes!!!"