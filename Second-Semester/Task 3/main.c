#include <assert.h>
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>


int main(int argc, char **argv) {
    // create initial array a
    static const int n = 100000;
    long *array = (long*) calloc(n, sizeof(long));
    assert(array);
    for (int i = 0; i < n; ++i) {
        array[i] = i;
    }

    // We will schedule for loop statically with batch size
    // 5000; thus we need 2 * (100000 / 5000 -1) == 38 buffered values
    // - 2 on every adjacent threads' memory blocks borders
    // Save scheme
    // - - - ...  -    -   save save  -   ...
    // * * * ...  *    *    *    *    *   ...
    // 0 1 2 ... 4998 4999 5000 5001 5002 ...
    static const int blockSize = 5000;
    static const int bufferSize = 2 * (n / blockSize - 1);
    long *bordersBuffer = (long*) calloc(bufferSize, sizeof(long));
    assert(bordersBuffer);
    for (int i = 0, end_index = bufferSize / 2; i < end_index; ++i) {
        bordersBuffer[2 * i] = array[blockSize * (i + 1)];
        bordersBuffer[2 * i + 1] = array[blockSize * (i + 1) + 1];
    }

    // Staff variables: counters and buffers
    int i = 1;
    int end_index = n - 1;
    int j = 0;
    int leftValue = 0;
    int rightValue = 0;
    int tmp = 0;
    // Work will be equal for all batches, thus we schedule statically with
    // the predefined batch size 5000. bordersBuffer and end_index are readonly
    // during the loop - we make them shared;
    // staff variables must be unique for all threads - we declare them private
    #pragma omp parallel for schedule(static, 5000) \
    shared(bordersBuffer, array, end_index) private(i) firstprivate(j, leftValue, rightValue, tmp)
    for (i = 1; i < end_index; ++i) {
        // leftValue is changed for blocks others than first
        if (j == 0 && i != 1) {
            leftValue = bordersBuffer[i / 2500 - 2];
        }
        rightValue = array[i + 1];
        // rightValue is taken from buffer in the end of batch
        if (j == 4999) {
            rightValue = bordersBuffer[i / 2500 - 1];
        }

        tmp = array[i];
        array[i] = (long)leftValue * array[i] * rightValue / 3;
        leftValue = tmp;

        // Batches are scheduled in cycle between threads;
        // after finishing one batch thread must drop internal counter to zero.
        // That effect is reached with % 5000 operation
        j = (j + 1) % 5000;
    }
    // don't forget about the last element modification
    array[end_index] = 0;

    // validation part
    FILE *file = fopen("result.dat", "r");
    if (file) {
        for (i = 0; i < n; ++i) {
            long tmp = 0.0;
            fscanf(file, "%ld", &tmp);
            if (array[i] != tmp) {
                fprintf(stderr, "Element on postition %d is incorrect: %ld != %ld\n", i, array[i], tmp);
                fclose(file);
                return 1;
            }
        }
        fclose(file);
    } else {
        fprintf(stderr, "Result data file isn't found, validation is impossible. Please generate file with generate_data.sh script\n");
    }

    free(array);
    free(bordersBuffer);

    return 0;
}
