//
// Created by denis on 13.03.2021.
//

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include "../merge.h"
#include "../data_generators.h"


// One thread implementation
int main() {
    const int number_of_elements = 100000000;
    int* array = (int*) calloc(number_of_elements, sizeof(int));
    get_random_array(number_of_elements, array);

    struct timeval begin, end;
    gettimeofday(&begin, 0);

    MergeSort(array, array + number_of_elements);

    gettimeofday(&end, 0);
    long seconds = end.tv_sec - begin.tv_sec;
    long microseconds = end.tv_usec - begin.tv_usec;
    double elapsed = seconds + microseconds * 1e-6;
    printf("Elapsed time equals %.20f\n", elapsed);

    free(array);
    return 0;
}
