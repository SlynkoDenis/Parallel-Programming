#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "utils.h"


double get_elapsed_time(struct timeval begin, struct timeval end) {
    long int seconds = end.tv_sec - begin.tv_sec;
    long int microseconds = end.tv_usec - begin.tv_usec;
    return seconds + microseconds * 1.0e-6;
}

void print_result(double result, struct timeval begin, struct timeval end,
                  struct timeval join_begin, struct timeval join_end) {
    double elapsed = get_elapsed_time(join_begin, join_end);
    printf("Time delay for pthread_join(): %.20f s\n", elapsed);

    elapsed = get_elapsed_time(begin, end);
    printf("By parallel algorithm: I = %.20f\n", result);
    printf("Time elapsed by parallel algorithm: %.20f s\n", elapsed);
}

double get_epsilon(int argc, char** argv) {
    assert(argv);
    assert(argc >= 1);
    if (argc == 1) {
        fprintf(stderr, "Epsilon is unspecified\n");
        exit(1);
    }
    double epsilon = atof(argv[1]);
    if (epsilon <= 0.0) {
        fprintf(stderr, "Invalid epsilon in input\n");
        exit(1);
    }
    return epsilon;
}

int is_one_thread_mode() {
    char* mode = getenv("MODE");
    if (mode && atoi(mode) == 0) {
        return 1;
    }
    return 0;
}

double pow_int(double base, unsigned int exp) {
    double res = 1.0;
    for (unsigned int i = 0; i < exp; ++i) {
        res *= base;
    }
    return res;
}
