#include <stdio.h>
#include <sys/time.h>

int main(int argc, char **argv) {
    int num_of_iterations = 20;

    double fact = 1.0;
    double e = 0.0;

    struct timeval begin, end;

    gettimeofday(&begin, 0);
    for (int i = 1; i < num_of_iterations; ++i) {
        e += 1 / fact;
        fact *= i;
    }
    gettimeofday(&end, 0);
    long seconds = end.tv_sec - begin.tv_sec;
    long microseconds = end.tv_usec - begin.tv_usec;
    double elapsed = seconds + microseconds * 1e-6;

    printf("%.20f\n", e);
    printf("Elapsed time equals %.20f\n", elapsed);

    return 0;
}
