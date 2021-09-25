#!/usr/bin/bash

get_usage_message() {
    echo "Usage: ./generate_data.sh [rows-in-a] [columns-in-a] [columns-in-b]"
    echo "Values 100 200 200 are used by default"
}

if [[ ("$#" == 1) && ($1 == "--help") ]]; then
    get_usage_message
    exit 0
fi

if [[ ("$#" != 0) && ("$#" != 3) ]]; then
    echo "ERROR: dimensions of matrices weren't provided"
    get_usage_message
    exit 1
fi

gcc generate_matrices.c matrix.c -o generator
if [[ "$#" == 0 ]]; then
    get_usage_message
    ./generator 100 200 200
else
    A_ROWS=$1
    A_COLUMNS=$2
    B_COLUMNS=$3

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
fi

rm generator
