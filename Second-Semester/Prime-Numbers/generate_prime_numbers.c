#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>


void sieveOfEratosthenes(int *numbers, int n) {
    assert(numbers);

    int prime = 2;
    while (1) {
        int tmp = prime * 2;
        if (tmp > n) {
            break;
        }

        while (tmp <= n) {
            numbers[tmp - 2] = -1;
            tmp += prime;
        }

        ++prime;
        for (; prime <= n && numbers[prime - 2] == -1; ++prime) {}
        if (prime * prime >= n) {
            break;
        } 
    }
}


int main(int argc, char **argv) {
    int n = 0;
    if (argc == 1) {
        fprintf(stderr, "Number of terms wasn't provided; using default value 100\n");
        n = 100;
    } else {
        n = atoi(argv[1]);
        if (n < 1) {
            fprintf(stderr, "Value from stdin: %d < 1\n", n);
            n = 100;
        }
    }

    if (n == 1) {
        return 0;
    }

    int *numbers = (int *) calloc(n - 1, sizeof(int));
    assert(numbers);
    numbers[0] = 2;
    for (int i = 3; i <= n; ++i) {
        if (i % 2 == 1) {
            numbers[i - 2] = i;
        } else {
            numbers[i - 2] = -1;
        }
    }

    struct timeval begin, end;
    gettimeofday(&begin, 0);

    sieveOfEratosthenes(numbers, n);

    gettimeofday(&end, 0);
    long seconds = end.tv_sec - begin.tv_sec;
    long microseconds = end.tv_usec - begin.tv_usec;
    double elapsed = seconds + microseconds * 1e-6;
    printf("Elapsed time equals %.20f\n", elapsed);


    FILE *file = fopen("result.dat", "w");
    assert(file);
    for (int i = 0, end_index = n - 1; i < end_index; ++i) {
        if (numbers[i] != -1) {
            fprintf(file, "%d ", numbers[i]);
        }
    }
    fclose(file);

    free(numbers);
    return 0;
}
