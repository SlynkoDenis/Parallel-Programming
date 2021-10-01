# Quick Sort with OpenMP

Program sorts one-dimensional array using parallel version of Quick Sort algorithm.
Algorithm uses only the original array, no buffers are used - **\* task condition is satisfied**.

Use either CMakeLists.txt or combination of build.sh & run.sh scripts
(run.sh script is required to be used with `-not` option denoting number of threads).
Use `generate_data.sh` script to generate `sample.dat` file with random 10000 integer values.
