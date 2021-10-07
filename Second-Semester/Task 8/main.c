#include <assert.h>
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

// Segments with size less than THRESHOLD will be processed in sequential mode
// in order not to exceed average number of treads
#define THRESHOLD 100


int initialize_array(int argc, char **argv, int *n, int **array) {
    /**
     * @brief initializes array with number from stdin or from file
     * @param argc          argc variable from main
     * @param argv          argv variable from main, used if filename was provided
     * @param n             variable for the length of array
     * @param array         target array
     * @returns 1 if an error occurred, 0 otherwise
     */
    if (argc == 2) {
        FILE *file = fopen(argv[1], "r");
        assert(file);
        fscanf(file, "%d", n);
        if (*n <= 0) {
            fprintf(stderr, "Invalid size of array: %d <= 0\n", *n);
            fclose(file);
            return 1;
        }

        *array = (int *) calloc(*n, sizeof(int));
        assert(*array);

        for (int i = 0, end_index = *n; i < end_index; ++i) {
            if (fscanf(file, "%d", *array + i) != 1) {
                fprintf(stderr, "Error while trying to read elenent %d from file\n", i);
                free(*array);
                fclose(file);
                return 1;
            }
        }
    } else {
        printf("Input firstly size of the array, then the elements one-by-one\n");
        scanf("%d", n);
        if (*n <= 0) {
            fprintf(stderr, "Invalid size of array: %d <= 0\n", *n);
            return 1;
        }

        *array = (int *) calloc(*n, sizeof(int));
        assert(*array);

        for (int i = 0, end_index = *n; i < end_index; ++i) {
            if (scanf("%d", *array + i) != 1) {
                fprintf(stderr, "Error while trying to read elenent %d\n", i);
                free(*array);
                return 1;
            }
        }
    }

    return 0;
}


int comparator(const void *x1, const void *x2) {
    return *(int*)x1 - *(int*)x2;
}


void staff_quicksort(int *array, int left, int right) {
    if (left >= right) {
        return;
    }

    int i = left;
    int j = right;
    int tmp = 0;
    // not the best way to ch0ose pivot, yet very handy
    int pivot = array[(left + right) / 2];

    // sort segment according to the chosen pivot
    while (i <= j) {
        while (array[i] < pivot) {
            ++i;
        }
        while (array[j] > pivot) {
            --j;
        }
        if (i <= j) {
            // Swap and continue the loop
            tmp = array[i];
            array[i] = array[j];
            array[j] = tmp;
            ++i;
            --j;
        }
    }

    if (right - left < THRESHOLD) {
        // We don't grow parallel task stack for better performance
        staff_quicksort(array, left, j);
        staff_quicksort(array, i, right);
    } else {
        #pragma omp task
        {
            staff_quicksort(array, left, j);
        }
        #pragma omp task
        {
            staff_quicksort(array, i, right);
        }
    }
}


void omp_quicksort(int *array, int n) {
    assert(array);
    #pragma omp parallel
    {
        // the first call must be done only in one thread
        #pragma omp single nowait
        {
            staff_quicksort(array, 0, n - 1);
        }
    }
}


int main(int argc, char **argv) {
    int n = 0;
    int *array = NULL;
    if (initialize_array(argc, argv, &n, &array)) {
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

    omp_quicksort(array, n);

    // time measurement
    gettimeofday(&end, 0);
    long seconds = end.tv_sec - begin.tv_sec;
    long microseconds = end.tv_usec - begin.tv_usec;
    double elapsed = seconds + microseconds * 1e-6;
    printf("Elapsed time for parallel version equals %.20fs\n", elapsed);

    // validation part (with time measurement)
    gettimeofday(&begin, 0);

    qsort(validation_copy, n, sizeof(int), comparator);

    gettimeofday(&end, 0);
    seconds = end.tv_sec - begin.tv_sec;
    microseconds = end.tv_usec - begin.tv_usec;
    elapsed = seconds + microseconds * 1e-6;
    printf("Elapsed time for stdlib version equals %.20fs\n", elapsed);

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
