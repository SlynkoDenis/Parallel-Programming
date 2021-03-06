#include <stdio.h>
#include <sys/time.h>
#include "../equation.h"


int main() {
    double tau = 0.05;
    double h = 0.05;

    struct equation eq = init();

    struct timeval begin, end;
    gettimeofday(&begin, 0);
    struct solution result = solve(eq, tau, h);
    gettimeofday(&end, 0);
    long seconds = end.tv_sec - begin.tv_sec;
    long microseconds = end.tv_usec - begin.tv_usec;
    double elapsed = seconds + microseconds * 1e-6;
    printf("Elapsed time equals %.20f\n", elapsed);

    printf("%d %d\n", result.K, result.M);

    return 0;
}
