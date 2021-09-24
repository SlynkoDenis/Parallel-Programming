#!/usr/bin/bash

get_usage_message() {
    echo "Usage: ./generate_data.sh [number: 1000 by default]"
}

if [[ "$#" == 0 ]]; then
    get_usage_message
    NUMBER=1000
else
    if [[ $1 == "--help" ]]; then
        get_usage_message
        exit 0
    else
        REG='^[0-9]+$'
        if ! [[ $1 =~ $re ]]; then
            get_usage_message
            exit 1
        fi
        NUMBER=$1
    fi
fi

gcc -Werror -Wall generate_prime_numbers.c -o tmp.out
./tmp.out $NUMBER
rm tmp.out
