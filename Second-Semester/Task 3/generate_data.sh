#!/bin/bash

gcc generate_data_file.c -o tmp.out
./tmp.out
rm tmp.out
