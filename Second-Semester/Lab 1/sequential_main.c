#include <stdio.h>
#include <stdlib.h>

#define ISIZE 1000
#define JSIZE 1000


int main(int argc, char **argv) {
    double a[ISIZE][JSIZE];
    int i = 0;
    int j = 0;
    FILE *ff = NULL;
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

    ff = fopen("result.txt","w");
    for(i = 0; i < ISIZE; ++i) {
        for (j = 0; j < JSIZE; ++j){
            fprintf(ff, "%f ", a[i][j]);
        }
        fprintf(ff, "\n");
    }
    fclose(ff);
    return 0;
}
