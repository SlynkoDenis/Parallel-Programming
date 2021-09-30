#include <assert.h>
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>


int isPrime(int number) {
    if (number == 1) {
        return 0;
    }
    if (number == 2) {
        return 1;
    }
    if (number % 2 == 0) {
        return 0;
    }

    for (int i = 3; i * i <= number; i += 2) {
        if (number % i == 0) {
            return 0;
        }
    }
    return 1;
}


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

    int i = 0;

    // time measurement
    struct timeval begin, end;
    gettimeofday(&begin, 0);

    // computations aren't equally loaded for different i (the bigger i the harder computation).
    // Guided scheduling is used to alleviate the effect
    #pragma omp parallel for schedule(guided) shared(numbers, n) private(i) default(none)
    for (i = 0; i < n - 1; ++i) {
        if (isPrime(i + 2)) {
            numbers[i] = i + 2;
        } else {
            numbers[i] = -1;
        }
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

    free(numbers);
    return 0;
}
