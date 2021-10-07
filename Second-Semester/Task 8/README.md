# Quick Sort with OpenMP

Program sorts one-dimensional array using parallel version of Quick Sort algorithm.
Program widely utilizes OMP tasks to implement recursion.
Algorithm uses only the original array, no buffers are used - **\* task condition is satisfied**.

Performance was measured for different types of arrays with `10000000` integer elements:

| Type of Array | stdlib (s) | omp (s) |
| :---: | :---: | :---: |
| Random | 2.036 | 0.399 |
| Random First Half | 1.183 | 0.339 |
| Random Second Half | 1.188 | 0.276 |
| Sequential | 0.396 | 0.125 |
| Reverse Sequential | 0.475 | 0.131 |

Use either CMakeLists.txt or combination of build.sh & run.sh scripts
(run.sh script is required to be used with `-not` option denoting number of threads).
Use `generate_data.sh` script to generate `sample.dat` file with random 10000 integer values.
