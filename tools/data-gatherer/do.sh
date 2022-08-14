#!/bin/sh
it=0
iterations=100
while [ $it -ne $iterations ]
do
    cd ../..
    echo Iteration $it
    ./server.sh > /dev/null
    ./build/four-souls-server game 2 0 > /dev/null
    rv=$?
    if [[ $rv -ne 0 ]]; then
        echo 'Iteration failed!'
        printf '\7'
        exit 1
    fi
    ((it=it+1))
    cd tools/data-gatherer
    python3 script.py
done