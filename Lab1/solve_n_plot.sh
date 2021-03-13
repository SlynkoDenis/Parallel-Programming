#!/usr/bin/bash

get_usage_message() {
    echo "Usage: ./solve_n_plot.sh [one, multi]"
}


MODE=$1

if [[ -z $MODE ]]; then
    echo "Error: solving mode is unspecified"
    get_usage_message
    exit 1
fi

if [[ $MODE = "one" ]]; then
    rm -rf build
    mkdir build
    cd build
    cmake ../one_thread || exit 1
    make -j4 || exit 1
    echo ""
    ./main

    ./../graph.py --dir=build || exit 1
    rm data.csv
else
    if [[ $MODE = "multi" ]]; then
        SCRIPT=$(readlink -f "$0")
        BASEDIR=$(dirname "$SCRIPT")

        cd multi_thread
        mpicc main.c ../equation.c -o main -lm || exit 1
        mpirun -np 4 $BASEDIR/multi_thread/main || exit 1

        cd $BASEDIR
        ./graph.py --dir=multi_thread || exit 1
        rm $BASEDIR/multi_thread/data.csv
    else
        echo "Error: solving mode is unresolved"
        get_usage_message
        exit 1  
    fi
fi
