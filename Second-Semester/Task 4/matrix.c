#include <assert.h>
#include <limits.h>
#include "matrix.h"
#include <stdlib.h>


Matrix createMatrix(int numberOfRows, int numberOfColumns) {
    Matrix matrix = {NULL, 0, 0};
    if ((long long)numberOfRows * numberOfColumns > (long long)INT_MAX) {
        return matrix;
    }

    matrix.elements = (int*)calloc(numberOfRows * numberOfColumns, sizeof(int));
    assert(matrix.elements);
    matrix.numberOfRows = numberOfRows;
    matrix.numberOfColumns = numberOfColumns;

    return matrix;
}


void initMatrixWithRandomValues(Matrix *matrix) {
    if (!matrix->elements) {
        return;
    }

    for (int i = 0, end_index = matrix->numberOfRows * matrix->numberOfColumns; i < end_index; ++i) {
        matrix->elements[i] = rand() % 100;
    }
}


void deleteMatrix(Matrix *matrix) {
    free(matrix->elements);
    matrix->elements = NULL;
    matrix->numberOfRows = 0;
    matrix->numberOfColumns = 0;
}


int setMatrixElement(Matrix *matrix, int row, int column, int new_element) {
    if (row >= matrix->numberOfRows || column >= matrix->numberOfColumns) {
        return -1;
    }
    matrix->elements[row * matrix->numberOfColumns + column] = new_element;
    return 0;
}


int getMatrixElement(Matrix *matrix, int row, int column) {
    if (row >= matrix->numberOfRows || column >= matrix->numberOfColumns) {
        return -1;
    }
    return matrix->elements[row * matrix->numberOfColumns + column];
}


int outputMatrixInFile(Matrix matrix, FILE *file) {
    assert(file);

    for (int i = 0, end_index = matrix.numberOfRows * matrix.numberOfColumns; i < end_index; ++i) {
        fprintf(file, "%d ", matrix.elements[i]);
    }

    return 0;
}


Matrix multiplyMatrices(Matrix A, Matrix B) {
    assert(A.numberOfColumns == B.numberOfRows);

    Matrix result = createMatrix(A.numberOfRows, B.numberOfColumns);
    for (int i = 0; i < result.numberOfRows; ++i) {
        for (int j = 0; j < result.numberOfColumns; ++j) {
            int elem = 0;
            for (int k = 0; k < B.numberOfRows; ++k) {
                elem += A.elements[i * A.numberOfColumns + k] * B.elements[k * B.numberOfColumns + j];
            }
            setMatrixElement(&result, i, j, elem);
        }
    }

    return result;
}


void loadMatrixFromFile(Matrix *matrix, FILE *file) {
    assert(matrix);
    assert(file);

    for (int i = 0, end_index = matrix->numberOfRows * matrix->numberOfColumns;
         i < end_index; ++i) {
        if (fscanf(file, "%d", matrix->elements + i) != 1) {
            fprintf(stderr, "Error while reading matrix data on position %d\n", i);
            for (int j = 0; j < end_index; ++j) {
                matrix->elements[j] = 0;
            }
            break;
        }
    }
}


int areMatricesEqual(Matrix lhs, Matrix rhs) {
    if (lhs.numberOfRows != rhs.numberOfRows || lhs.numberOfColumns != rhs.numberOfColumns) {
        return 0;
    }

    for (int i = 0, end_index = lhs.numberOfRows * lhs.numberOfColumns; i < end_index; ++i) {
        if (lhs.elements[i] != rhs.elements[i]) {
            return 0;
        }
    }

    return 1;
}
