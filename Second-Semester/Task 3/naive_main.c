#include <assert.h>
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>


int main(int argc, char **argv) {
    // create initial array a
    static const int n = 100000;
    int *array = (int*) calloc(n, sizeof(int));
    assert(array);
    for (int i = 0; i < n; ++i) {
        array[i] = i;
    }

    // prepare buffer array b
    long *bArray = (long*) calloc(n, sizeof(long));
    assert(bArray);
    bArray[0] = bArray[n - 1] = 0;

    int i = 1;
    int end_index = n - 1;

    // access to arrays b and a can be shared because different threads doesn't modify
    // each others' parts of arrays, thus race condition won't arise
    #pragma omp parallel for shared(bArray, array, end_index) private(i)
    for (i = 1; i < end_index; ++i) {
        bArray[i] = (long)array[i - 1] * array[i] * array[i + 1] / 3;
    }

    // validation part
    FILE *file = fopen("result.dat", "r");
    if (file) {
        for (i = 0; i < n; ++i) {
            long tmp = 0.0;
            fscanf(file, "%ld", &tmp);
            if (bArray[i] != tmp) {
                fprintf(stderr, "Element on postition %d is incorrect: %ld ! %ld\n", i, bArray[i], tmp);
                fclose(file);
                return 1;
            }
        }
        fclose(file);
    } else {
        fprintf(stderr, "Result data file isn't found, validation is impossible. Please generate file with generate_data.sh script\n");
    }

    free(array);
    free(bArray);

    return 0;
}
