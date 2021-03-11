//
// Created by denis on 10.03.2021.
//

#include <assert.h>
#include <stdlib.h>
#include "mpi.h"
#include "merge.h"

#define SEND_TAG 2001

void get_random_array(int number_of_elements, int* array) {
    assert(array);

    for (int i = 0; i < number_of_elements; ++i) {
        array[i] = rand();
    }
}


void get_saw(int number_of_elements, int* array) {
    assert(array);

    int l = 10;
    int j = 0;
    while (1) {
        for (int i = 0; i < l; ++i) {
            if (j * l + i >= number_of_elements) {
                return;
            }
            array[j * l + i] = i;
        }
        ++j;
    }
}


void get_check_mark(int number_of_elements, int* array) {
    assert(array);

    int middle = number_of_elements / 2;
    for (int i = 0; i < middle; ++i) {
        array[i] = middle - i;
    }
    for (int i = middle; i < number_of_elements; ++i) {
        array[i] = i - middle;
    }
}


void get_consecutive_numbers(int number_of_elements, int* array) {
    assert(array);

    for (int i = 0; i < number_of_elements; ++i) {
        array[i] = i;
    }
}


void get_reversed_consecutive_numbers(int number_of_elements, int* array) {
    assert(array);

    for (int i = 0; i < number_of_elements; ++i) {
        array[i] = number_of_elements - i;
    }
}


int main(int argc, char** argv) {
    const int number_of_elements = 40000000;
    int* array = (int*) calloc(number_of_elements, sizeof(int));
    assert(array);
    get_random_array(number_of_elements, array);

    MPI_Status status;
    int root_process = 0;
    int my_id = 0;
    int num_procs = 0;
    int ierr = MPI_Init(&argc, &argv);
    ierr = MPI_Comm_rank(MPI_COMM_WORLD, &my_id);
    ierr = MPI_Comm_size(MPI_COMM_WORLD, &num_procs);

    // average number of elements to process on every thread
    int avg_element_per_process = number_of_elements / num_procs + 1;
    if (my_id == root_process) {
        MergeSort(array, array + avg_element_per_process);

        int* buffer = (int*) calloc(number_of_elements, sizeof(int));
        assert(buffer);

        for (int i = 1; i < num_procs - 1; ++i) {
            ierr = MPI_Recv(array + i * avg_element_per_process, avg_element_per_process, MPI_INT, i,
                            MPI_ANY_TAG, MPI_COMM_WORLD, &status);
            Merge(array, array + avg_element_per_process * i, array + avg_element_per_process * i,
                  array + avg_element_per_process * (i + 1), buffer);
        }
        ierr = MPI_Recv(array + (num_procs - 1) * avg_element_per_process,
                        number_of_elements - avg_element_per_process * (num_procs - 1), MPI_INT, num_procs - 1,
                        MPI_ANY_TAG, MPI_COMM_WORLD, &status);
        Merge(array, array + avg_element_per_process * (num_procs - 1),
              array + avg_element_per_process * (num_procs - 1),
              array + number_of_elements, buffer);

        //// MergeNParts merges num_procs batches at one call, however, it increases execution time
        // MergeNParts(array, number_of_elements, avg_element_per_process, buffer);

        free(buffer);
    } else {
        int start = avg_element_per_process * my_id;
        int end = avg_element_per_process * (my_id + 1);
        if (end > number_of_elements) {
            end = number_of_elements;
        }

        MergeSort(array + start, array + end);

        ierr = MPI_Send(array + start, end - start, MPI_INT, root_process,
                        SEND_TAG, MPI_COMM_WORLD);
    }

    free(array);
    ierr = MPI_Finalize();
    return 0;
}


//// One thread implementation
//int main() {
//    const int number_of_elements = 100000000;
//    int* array = (int*) calloc(number_of_elements, sizeof(int));
//    get_random_array(number_of_elements, array);
//
//    MergeSort(array, array + number_of_elements);
//
//    free(array);
//    return 0;
//}
