//
// Created by denis on 10.03.2021.
//

#include <assert.h>
#include <math.h>
#include <stdlib.h>
#include "merge.h"


int* BinarySearch(int* first, int* last, const int to_find) {
    assert(last >= first);
    if (last - first == 1) {
        return first;
    }

    int* tmp_iter = first + (last - first) / 2;
    while (last - first > 1) {
        if (*tmp_iter < to_find) {
            first = tmp_iter;
            tmp_iter += (last - first) / 2;
        } else {
            last = tmp_iter;
            tmp_iter -= (last - first) / 2;
        }
    }

    return first;
}


void InsertionSort(int* first, int* last) {
    if (last - first <= 1)
        return;

    for (unsigned int i = 1, end_index = last - first; i < end_index; ++i) {
        int tmp_elem = *(first + i);

        int* pos = BinarySearch(first, first + i, tmp_elem);

        unsigned int tmp_pos = (*pos <= tmp_elem) ? end_index - (last - pos) + 1 : end_index - (last - pos);
        for (unsigned int j = i; j > tmp_pos; --j) {
            *(first + j) = *(first + j - 1);
        }
        *(first + tmp_pos) = tmp_elem;
    }
}


int find_min(int** starts, int* numbers, int n) {
    int number = 0;
    int i = 0;
    for (; i < n; ++i) {
        if (numbers[i] >= 0) {
            number = *starts[i];
            break;
        }
    }
    if (i == n) {
        return -1;
    }
    int index = i;

    for (; i < n; ++i) {
        if (numbers[i] >= 0) {
            if (*starts[i] < number) {
                number = *starts[i];
                index = i;
            }
        }
    }

    return index;
}


void Merge(int* first_begin, int* first_end, int* second_begin, int* second_end, int* buffer) {
    assert(first_end >= first_begin || second_end >= second_begin);

    int* tmp = first_begin;
    int* tmp_buffer = buffer;
    while (tmp != first_end) {
        *tmp_buffer = *tmp;
        ++tmp;
        ++tmp_buffer;
    }

    while (buffer != tmp_buffer && second_begin != second_end) {
        if (*buffer <= *second_begin) {
            *first_begin = *buffer;
            ++buffer;
        } else {
            *first_begin = *second_begin;
            ++second_begin;
        }

        ++first_begin;
    }

    if (second_begin < second_end) {
        while (second_begin != second_end) {
            *first_begin = *second_begin;
            ++second_begin;
            ++first_begin;
        }
    } else if (buffer < tmp_buffer) {
        while (buffer != tmp_buffer) {
            *first_begin = *buffer;
            ++first_begin;
            ++buffer;
        }
    }
}


void MergeNParts(int* array, int length, int step, int* buffer) {
    assert(array);
    assert(buffer);

    int n = (int)ceil((float)(length) / step);
    int** starts = (int**) calloc(n, sizeof(int*));
    assert(starts);
    int** ends = (int**) calloc(n, sizeof(int*));
    assert(ends);
    for (int i = 0; i < n - 1; ++i) {
        starts[i] = buffer + i * step;
        ends[i] = buffer + (i + 1) * step;
    }
    starts[n - 1] = buffer + (n - 1) * step;
    ends[n - 1] = buffer + length;

    int* numbers = (int*) calloc(n, sizeof(int));
    assert(numbers);
    for (int i = 0; i < n; ++i) {
        numbers[i] = i;
    }

    while (1) {
        int full = 1;
        for (int i = 0; i < n; ++i) {
            if (starts[i] == ends[i]) {
                numbers[i] = -1;
            } else {
                full = 0;
            }
        }
        if (full) {
            break;
        }

        int index = find_min(starts, numbers, n);
        *array = *starts[index];
        ++array;
        ++starts[index];
    }

    free(starts);
    free(ends);
    free(numbers);
}


void MergeSort(int* first, int* last) {
    if (last - first <= 1)
        return;

    unsigned int sz = (unsigned)(last - first);
    if (sz <= 12) {
        InsertionSort(first, last);
        return;
    }

    unsigned int i = 0;
    for (; i + 15 < sz; i += 8) {
        InsertionSort(first + i, first + i + 8);
    }
    InsertionSort(first + i, last);

    int* buffer = (int*) calloc((unsigned int)(pow(2.0, log2(sz))), sizeof(int));
    assert(buffer);
    i = 8;
    for (; i < sz; i *= 2) {
        for (unsigned int j = 0; j < sz / i; j += 2) {
            Merge(first + i * j, first + i * (j + 1), first + i * (j + 1),
                  first + (int) fmin(i * (j + 2), sz), buffer);
        }
    }
}
