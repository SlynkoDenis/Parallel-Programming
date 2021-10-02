#!/bin/bash -e

gcc sequential_main.c -o sq -lm
./sq
rm sq
