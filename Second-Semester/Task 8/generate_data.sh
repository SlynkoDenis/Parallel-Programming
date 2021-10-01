#!/bin/bash

gcc generate_data.c -o gen
./gen
rm gen
