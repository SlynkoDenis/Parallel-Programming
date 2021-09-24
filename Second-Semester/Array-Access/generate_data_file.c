#include <assert.h>
#include <stdio.h>
#include <stdlib.h>


int main(int argc, char **argv) {
    static const int n = 100000;
    int *array = (int*) calloc(n, sizeof(int));
    assert(array);
    for (int i = 0; i < n; ++i) {
        array[i] = i;
    }

    float *bArray = (float*) calloc(n, sizeof(float));
    assert(bArray);
    bArray[0] = bArray[n - 1] = 0.0F;
    
    int i = 1;
    int end_index = n - 1;

    for (i = 1; i < end_index; ++i) {
        bArray[i] = (array[i - 1] * array[i] * array[i + 1]) / 3.0F;
    }

    FILE *file = fopen("result.dat", "w");
    assert(file);
    for (i = 0; i < n; ++i) {
        fprintf(file, "%f ", bArray[i]);
    }
    fclose(file);

    free(array);
    free(bArray);

    return 0;
}
