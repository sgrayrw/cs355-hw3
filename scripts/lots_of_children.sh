#!/usr/bin/env sh
echo "Firing off 100 child processes"
for (( i=0; i < 10; ++i ))
do
    echo "$i"
done