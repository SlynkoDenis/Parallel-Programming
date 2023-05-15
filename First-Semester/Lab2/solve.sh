#!/usr/bin/bash

get_usage_message() {
    echo "Usage: ./solve.sh [one, multi] [epsilon]"
    echo "Note: one-thread method uses fixed integration step, which is made for showing dummy approach disadvantages."
}

compile() {
    gcc main.c integral_multithreaded.c integral_singlethreaded.c utils.c -lpthread -lm -O3
}

MODE=$1
EPSILON=$2

if [[ -z $MODE ]]; then
    echo "Error: solving mode is unspecified"
    get_usage_message
    exit 1
fi

if [[ -z $EPSILON ]]; then
    echo "Error: epsilon is unspecified"
    get_usage_message
    exit 1
fi

if [[ $MODE = "one" ]]; then
    export MODE=0
    compile
    ./a.out $EPSILON
    unset MODE
else
    if [[ $MODE = "multi" ]]; then
        compile
        ./a.out $EPSILON
    else
        echo "Error: solving mode is unresolved"
        get_usage_message
        exit 1
    fi
fi
