#!/bin/bash

get_usage_message() {
    echo "Usage: ./build.sh [filename: a.out by default]"
}

FILENAME=$1

if [[ $FILENAME == "--help" ]]; then
    get_usage_message
    exit 0
fi

if [[ -z $FILENAME ]]; then
    FILENAME=a.out
fi

gcc -fopenmp main.c matrix.c -o $FILENAME
