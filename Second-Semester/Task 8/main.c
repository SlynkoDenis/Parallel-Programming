#include <assert.h>
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>


int initialize_array(int argc, char **argv, int *n, int *array) {
    if (argc == 2) {
        FILE *file = fopen(argv[1], "r");
        assert(file);
        fscanf(file, "%d", &n);
        if (n <= 0) {
            fprintf(stderr, "Invalid size of array: %d <= 0\n", n);
            fclose(file);
            return 1;
        }

        array = (int *) calloc(n, sizeof(int));
        assert(array);

        for (int i = 0; i < n; ++i) {
            if (fscanf(file, "%d", array + i) != 1) {
                fprintf(stderr, "Error while trying to read elenent %d from file\n", i);
                free(array);
                fclose(file);
                return 1;
            }
        }
    } else {
        scanf("%d", &n);
        if (n <= 0) {
            fprintf(stderr, "Invalid size of array: %d <= 0\n", n);
            return 1;
        }

        array = (int *) calloc(n, sizeof(int));
        assert(array);

        for (int i = 0; i < n; ++i) {
            if (scanf("%d", array + i) != 1) {
                fprintf(stderr, "Error while trying to read elenent %d\n", i);
                free(array);
                return 1;
            }
        }
    }
}


int comparator(const void *x1, const void *x2) {
    return *(int*)x1 - *(int*)x2;
}


int main(int argc, char **argv) {
    int n = 0;
    int *array = NULL;
    if (initialize_array(argc, argv, &n, array)) {
        return 1;
    }

    // copy of the array for further validation
    int *validation_copy = (int *) calloc(n, sizeof(int));
    assert(validation_copy);
    for (int i = 0; i < n; ++i) {
        validation_copy[i] = array[i];
    }

    // time measurement
    struct timeval begin, end;
    gettimeofday(&begin, 0);

    int *buffer = (int *) calloc(n, sizeof(int));
    assert(buffer);

    int i = 0;
    

    // time measurement
    gettimeofday(&end, 0);
    long seconds = end.tv_sec - begin.tv_sec;
    long microseconds = end.tv_usec - begin.tv_usec;
    double elapsed = seconds + microseconds * 1e-6;
    printf("Elapsed time equals %.20f\n", elapsed);

    // validation part
    qsort(validation_copy, n, sizeof(int), comparator);
    for (int i = 0; i < n; ++i) {
        if (validation_copy[i] != array[i]) {
            fprintf(stderr, "Element on postition %d is incorrect: %d != %d\n", i, array[i], validation_copy[i]);
            break;
        }
    }

    free(validation_copy);
    free(array);
    return 0;
}
