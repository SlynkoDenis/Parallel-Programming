# Sum of Number with OpenMP

Program sums numbers from 1 to the given in multi-thread manner and outputs the number itself.
Program requires number N as an input (100 is used by default); it uses reduction clause
and guided scheduling for efficient computations - **\* task conditions are satisfied**.

Use either CMakeLists.txt or combination of build.sh & run.sh scripts
(run.sh script is required to be used with `-not` option denoting number of threads)
