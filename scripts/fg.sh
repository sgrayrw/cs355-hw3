# run with `$source fg.sh` in mysh
# test builtin fg()

echo "cat - &"
sleep 1
cat - &
sleep 1

echo "cat - &"
sleep 1
cat - &
sleep 1

echo "cat - &"
sleep 1
cat - &
sleep 1

echo "cat - &"
sleep 1
cat - &
sleep 1

echo "jobs"
sleep 1
jobs
sleep 1

echo "fg (need ctrl-d)"
sleep 1
fg
sleep 1

echo "fg %2 (need ctrl-d)"
sleep 1
fg %
sleep 1

echo "jobs"
sleep 1
jobs
sleep 1

echo "fg % (need ctrl-d)"
sleep 1
fg %
sleep 1

echo "jobs"
sleep 1
jobs
sleep 1

echo "fg %0"
sleep 1
fg %0
sleep 1

echo "fg%1"
sleep 1
fg %0
sleep 1

echo "fg %asdf"
sleep 1
fg %0
sleep 1

echo "fg  %   asdf"
sleep 1
fg %0
sleep 1

echo "fg !@#$%^"
sleep 1
fg !@#$%^
sleep 1

echo "fg asdf"
sleep 1
fg asdf
sleep 1

echo "fg 1"
sleep 1
fg 1
sleep 1

echo "fg %1 (need ctrl-d)"
sleep 1
fg %1
sleep 1

echo "mysh survived fg!!!"
