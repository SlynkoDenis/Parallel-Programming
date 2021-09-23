#!/usr/bin/bash

gcc generate_matrices.c matrix.c -o generator
./generator
mv a_matrix.dat b_matrix.dat c_matrix.dat cmake-build-debug
