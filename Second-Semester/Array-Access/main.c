#include <assert.h>
#include <omp.h>
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

    #pragma omp parallel for shared(bArray, array, end_index) private(i)
    for (i = 1; i < end_index; ++i) {
        bArray[i] = (array[i - 1] * array[i] * array[i + 1]) / 3.0F;
    }

    FILE *file = fopen("result.dat", "r");
    if (file) {
        for (i = 0; i < n; ++i) {
            float tmp = 0.0F;
            fscanf(file, "%f", &tmp);
            if (tmp != bArray[i]) {
                fprintf(stderr, "Element on postition %d is incorrect: %f ! %f", i, bArray[i], tmp);
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
