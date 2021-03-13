//
// Created by denis on 13.03.2021.
//

#include <assert.h>
#include <stdlib.h>
#include "data_generators.h"


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
