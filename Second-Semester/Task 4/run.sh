#!/bin/bash

get_usage_message() {
    echo "Usage: ./run.sh -not [--number-of-threads] NUMBER_OF_THREADS -n [--name] FILENAME [a.out by default] ROWS-IN-A COLUMNS-IN-A COLUMNS-IN-B"
    echo "Values 100 200 200 are used by default"
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

if [[ ($FIRST_ARG == "-not") || ($FIRST_ARG == "--number-of-threads") ]]; then
    REG='^[0-9]+$'
    if ! [[ $FIRST_ARG_VALUE =~ $re ]]; then
        FIRST_ARG_VALUE=0
    fi
else
    echo "ERROR: number-of-threads parameter wasn't provided"
    get_usage_message
    exit 1
fi


if [[ "$#" == 2 ]]; then
    get_usage_message
    if [[ $FIRST_ARG_VALUE != "0" ]]; then
        OMP_NUM_THREADS=$FIRST_ARG_VALUE ./a.out 100 200 200
    else
        ./a.out 100 200 200
    fi
else
    shift
    shift
    if [[ ($1 != "-n") && ($1 != "--name") ]]; then
        if [[ $FIRST_ARG_VALUE != "0" ]]; then
            OMP_NUM_THREADS=$FIRST_ARG_VALUE ./a.out "$@"
        else
            ./a.out "$@"
        fi
    else
        FILENAME=$2
        shift
        shift
        if [[ $FIRST_ARG_VALUE != "0" ]]; then
            OMP_NUM_THREADS=$FIRST_ARG_VALUE ./${FILENAME} "$@"
        else
            ./${FILENAME} "$@"
        fi
    fi
fi
