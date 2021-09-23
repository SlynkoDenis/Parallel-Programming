#include <omp.h>
#include "matrix.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>


int main(int argc, char **argv) {
    srand((unsigned int)time(NULL) / 2);

    int rowsInA = 2;
    int columnsInA = 3;
    int columnsInB = 3;

    Matrix A = createMatrix(rowsInA, columnsInA);
    FILE *aDescriptor = fopen("a_matrix.dat", "r");
    loadMatrixFromFile(&A, aDescriptor);
    fclose(aDescriptor);

    Matrix B = createMatrix(columnsInA, columnsInB);
    FILE *bDescriptor = fopen("b_matrix.dat", "r");
    loadMatrixFromFile(&B, bDescriptor);
    fclose(bDescriptor);

    Matrix C = createMatrix(rowsInA, columnsInB);

    int rowIndex = 0;
    int columnIndex = 0;
    int newElement = 0;
    int i = 0;
    #pragma omp parallel for collapse(2) schedule(runtime) shared(A, B, C) private(rowIndex, columnIndex, newElement, i) default(none)
    for (rowIndex = 0; rowIndex < C.numberOfRows; ++rowIndex) {
        for (columnIndex = 0; columnIndex < C.numberOfColumns; ++columnIndex) {
            newElement = 0;
            // printf("Got indeces (%d, %d) in thread %d\n", rowIndex, columnIndex, omp_get_thread_num());
            for (i = 0; i < A.numberOfColumns; ++i) {
                newElement += getMatrixElement(&A, rowIndex, i) * getMatrixElement(&B, i, columnIndex);
            }
            setMatrixElement(&C, rowIndex, columnIndex, newElement);
        }
    }

    Matrix oneThreadCalculated = createMatrix(rowsInA, columnsInB);
    FILE *file = fopen("c_matrix.dat", "r");
    loadMatrixFromFile(&oneThreadCalculated, file);
    if (!areMatricesEqual(C, oneThreadCalculated)) {
        fprintf(stderr, "Result is incorrect!\n");
    }
    deleteMatrix(&oneThreadCalculated);

    deleteMatrix(&A);
    deleteMatrix(&B);
    deleteMatrix(&C);

    return 0;
}
