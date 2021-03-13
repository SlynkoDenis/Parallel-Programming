//
// Created by denis on 10.03.2021.
//

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include "mpi.h"
#include "../merge.h"
#include "../data_generators.h"

#define SEND_TAG 2001

int main(int argc, char** argv) {
    const int number_of_elements = 40000000;
    int* array = (int*) calloc(number_of_elements, sizeof(int));
    assert(array);
    get_saw(number_of_elements, array);

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
        struct timeval begin, end;
        gettimeofday(&begin, 0);

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

        gettimeofday(&end, 0);
        long seconds = end.tv_sec - begin.tv_sec;
        long microseconds = end.tv_usec - begin.tv_usec;
        double elapsed = seconds + microseconds * 1e-6;
        printf("Elapsed time equals %.20f\n", elapsed);

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
