#!/usr/bin/bash

get_usage_message() {
    echo "Usage: ./run.sh -n NUMBER_OF_THREADS [rows-in-a] [columns-in-a] [columns-in-b]"
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

if [[ ($FIRST_ARG == "-n") || ($FIRST_ARG == "--number-of-threads") ]]; then
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
    A_ROWS=$3
    A_COLUMNS=$4
    B_COLUMNS=$5

    REG='^[0-9]+$'
    if ! [[ $A_ROWS =~ $re ]]; then
        echo "ERROR: incorrect value was used as number of rows in A"
        A_ROWS=100
    fi
    if ! [[ $A_COLUMNS =~ $re ]]; then
        echo "ERROR: incorrect value was used as number of columns in A"
        A_COLUMNS=200
    fi
    if ! [[ $B_COLUMNS =~ $re ]]; then
        echo "ERROR: incorrect value was used as number of columns in B"
        B_COLUMNS=200
    fi
    if [[ $FIRST_ARG_VALUE != "0" ]]; then
        OMP_NUM_THREADS=$FIRST_ARG_VALUE ./a.out $A_ROWS $A_COLUMNS $B_COLUMNS
    else
        ./a.out $A_ROWS $A_COLUMNS $B_COLUMNS
    fi
fi
