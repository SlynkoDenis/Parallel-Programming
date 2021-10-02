#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#define ISIZE 1000
#define JSIZE 1000


int main(int argc, char **argv) {
    double a[ISIZE][JSIZE];
    int i = 0;
    int j = 0;
    FILE *ff = NULL;

    // time measurement
    struct timeval begin, end;
    gettimeofday(&begin, 0);

    for (i = 0; i < ISIZE; ++i){
        for (j = 0; j < JSIZE; ++j){
            a[i][j] = 10 * i + j;
        }
    }
    for (i = 0; i < ISIZE - 4; ++i) {
        for (j = 5; j < JSIZE; ++j){
            a[i][j] = sin(0.00001 * a[i + 4][j - 5]);
        }
    }

    // time measurement
    gettimeofday(&end, 0);
    long seconds = end.tv_sec - begin.tv_sec;
    long microseconds = end.tv_usec - begin.tv_usec;
    double elapsed = seconds + microseconds * 1e-6;
    printf("Time elapsed %.20fs\n", elapsed);

    ff = fopen("result.txt", "w");
    for (i = 0; i < ISIZE; ++i) {
        for (j = 0; j < JSIZE; ++j){
            fprintf(ff, "%f ", a[i][j]);
        }
        fprintf(ff, "\n");
    }

    fclose(ff);
    return 0;
}
