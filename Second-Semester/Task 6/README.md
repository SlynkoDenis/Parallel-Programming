# Prime Numbers calculation with OpenMP

Program finds all prime number from 1 to the given N.
Sieve of Eratosthenes algorithm is used for testing data generation and in main program -
**\* task condition is satisfied**. The algorithm was chosen because of lighweight implementation
yet reasonably efficient complexity of `O(N * log log N)`. In the naive implementation (file `naive_main.c`)
an approach to successively check all the numbers was considered. The complexity for such method is:

![complexity](https://latex.codecogs.com/svg.latex?O(\sum_{k=2}^{N}\frac{\sqrt{k}}{2}),)

which is worser than sieve of Eratosthenes (on experiment runtime for naive approach with `N = 10 000 000`
was by an order of magnitude worser than sieve).

**All complexities are given for one-thread implementations.**

Use either CMakeLists.txt or combination of build.sh & run.sh scripts
(run.sh script is required to be used with `-not` option denoting number of threads).
Use `generate_data.sh [n]` for generation of `result.dat` file for validation.
