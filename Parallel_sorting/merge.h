//
// Created by denis on 10.03.2021.
//

#pragma once
#ifndef PARALLEL_SORTING_MERGE_H
#define PARALLEL_SORTING_MERGE_H


int* BinarySearch(int* first, int* last, int to_find);
void InsertionSort(int* first, int* last);
void Merge(int* first_begin, int* first_end, int* second_begin, int* second_end, int* buffer);
void MergeNParts(int* array, int length, int number_of_parts, int* buffer);
void MergeSort(int* first, int* last);

#endif //PARALLEL_SORTING_MERGE_H
