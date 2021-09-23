#!/usr/bin/bash

gcc generate_matrices.c matrix.c -o generator
./generator
rm generator
