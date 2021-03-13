#include <assert.h>
#include <stdio.h>
#include <sys/time.h>
#include "../equation.h"


void write_csv(const char* name, struct solution result, double tau, double h) {
    FILE* fd = fopen(name, "wb");
    assert(fd);

    for (unsigned int i = 0; i < result.K; ++i) {
        for (unsigned int j = 0; j < result.M; ++j) {
            fprintf(fd, "%.20f,", result.values[i * result.M + j]);
        }
    }
    fprintf(fd, "%d,%.20f,%d,%.20f", (int)result.K, tau, (int)result.M, h);

    fclose(fd);
}


int main() {
    double tau = 0.001;
    double h = 0.05;

    struct equation eq = init();

    struct timeval begin, end;
    gettimeofday(&begin, 0);
    struct solution result = solve(eq, tau, h);
    gettimeofday(&end, 0);
    long seconds = end.tv_sec - begin.tv_sec;
    long microseconds = end.tv_usec - begin.tv_usec;
    double elapsed = seconds + microseconds * 1e-6;
    printf("Solution time equals %.20f\n", elapsed);

    write_csv("data.csv", result, tau, h);
    delete(result);

    return 0;
}
