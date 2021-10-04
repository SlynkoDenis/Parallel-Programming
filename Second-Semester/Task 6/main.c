#include <assert.h>
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>


int main(int argc, char **argv) {
    int n = 0;
    if (argc == 1) {
        fprintf(stderr, "Number of terms wasn't provided; using default value 1000\n");
        n = 1000;
    } else {
        n = atoi(argv[1]);
        if (n < 1) {
            fprintf(stderr, "Invalid value from stdin: %d < 1\n", n);
            n = 1000;
        }
    }

    if (n == 1) {
        return 0;
    }

    int *numbers = (int *) calloc(n - 1, sizeof(int));
    assert(numbers);

    // time measurement
    struct timeval begin, end;
    gettimeofday(&begin, 0);

    int i = 0;
    // Initializing array part
    numbers[0] = 2;
    #pragma omp parallel for shared(numbers, n) private(i) default(none)
    for (i = 3; i <= n; ++i) {
        if (i % 2 == 1) {
            numbers[i - 2] = i;
        } else {
            numbers[i - 2] = -1;
        }
    }

    // Main part of Sieve of Eratosthenes algorithm
    int prime = 3;
    while (1) {
        int tmp = prime * prime;
        if (tmp > n) {
            break;
        }

        // Work is computationally equal for all batches,
        // no special scheduling is needed
        i = tmp;
        #pragma omp parallel for shared(numbers, n, tmp, prime) private(i) default(none)
        for (i = tmp; i < n; i += prime) {
            numbers[i - 2] = -1;
        }

        ++prime;
        for (; prime <= n && numbers[prime - 2] == -1; ++prime) {}
    }

    // time measurement
    gettimeofday(&end, 0);
    long seconds = end.tv_sec - begin.tv_sec;
    long microseconds = end.tv_usec - begin.tv_usec;
    double elapsed = seconds + microseconds * 1e-6;
    printf("Elapsed time equals %.20f\n", elapsed);

    // validation part
    FILE *file = fopen("result.dat", "r");
    if (file) {
        for (int i = 0, end_index = n - 1; i < end_index; ++i) {
            if (numbers[i] != -1) {
                int tmp = 0;
                if (fscanf(file, "%d", &tmp) != 1) {
                    fprintf(stderr, "Failed to retrieve number from file on %dth iteration\n", i);
                    return 1;
                }
                if (tmp != numbers[i]) {
                    fprintf(stderr, "Numbers %d and %d aren't matched\n", tmp, numbers[i]);
                    free(numbers);
                    return 1;
                }
            }
        }
        fclose(file);
    } else {
        fprintf(stderr, "Result data file isn't found, validation is impossible. Please generate file with generate_data.sh script\n");
    }

    // count total amount of primes
    int number_of_primes = 0;
    for (int i = 0, end_index = n - 1; i < end_index; ++i) {
        if (numbers[i] != -1) {
            ++number_of_primes;
        }
    }
    printf("Totally got %d prime numbers less or equal than %d\n", number_of_primes, n);

    free(numbers);
    return 0;
}
