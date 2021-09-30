#!/bin/bash -e

get_usage_message() {
    echo "Usage: ./build.sh SOURCEFILE FILENAME [a.out by default]"
}

SOURCEFILE=$1

if [[ -z $SOURCEFILE ]]; then
    echo "ERROR: source file name wasn't provided"
    get_usage_message
    exit 1
fi

if [[ $SOURCEFILE == "--help" ]]; then
    get_usage_message
    exit 0
fi

FILENAME=$2

if [[ -z $FILENAME ]]; then
    FILENAME=a.out
fi

gcc -fopenmp -Wall -Werror $SOURCEFILE -lm -o $FILENAME
