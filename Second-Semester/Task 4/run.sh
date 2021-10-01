#!/bin/bash

get_usage_message() {
    echo "Usage: ./run.sh -not [--number-of-threads] NUMBER_OF_THREADS -n [--name] FILENAME [a.out by default] ROWS-IN-A COLUMNS-IN-A COLUMNS-IN-B"
}

if [[ ("$#" == 1) && ($1 == "--help") ]]; then
  get_usage_message
  exit 0
fi

if [[ "$#" < 2 ]]; then
    echo "ERROR: number-of-threads parameter wasn't provided"
    get_usage_message
    exit 1
fi

FIRST_ARG=$1
FIRST_ARG_VALUE=$2
FILENAME=a.out

if [[ ($FIRST_ARG == "-not") || ($FIRST_ARG == "--number-of-threads") ]]; then
    shift
    shift
    if [[ "$#" -ge 2 ]] && [[ ($1 == "-n") || ($1 == "--name") ]]; then
        FILENAME=$2
        shift
        shift
    fi
else
    if [[ ($FIRST_ARG == "-n") || ($FIRST_ARG == "--name") ]]; then
        FILENAME=$FIRST_ARG_VALUE

        shift
        shift
        if [[ ("$#" < 2) || ( ($1 != "-not") && ($1 != "--number-of-threads") ) ]]; then
            echo "ERROR: number-of-threads parameter wasn't provided"
            get_usage_message
            exit 1
        fi
        FIRST_ARG_VALUE=$2
        shift
        shift
    else
        echo "ERROR: number-of-threads parameter wasn't provided"
        get_usage_message
        exit 1
    fi
fi

if [[ ! -e $FILENAME ]]; then
    echo "ERROR: executable file ${FILENAME} doesn't exist"
    get_usage_message
    exit 1
fi

OMP_NUM_THREADS=$FIRST_ARG_VALUE ./${FILENAME} "$@"
