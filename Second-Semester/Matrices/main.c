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

    FILE *aDescriptor = fopen("a_matrix.dat", "r");
    if (!aDescriptor) {
        fprintf(stderr, "A matrix file isn't found, execution is impossible\n");
        return 1;
    }
    Matrix A = createMatrix(rowsInA, columnsInA);
    loadMatrixFromFile(&A, aDescriptor);
    fclose(aDescriptor);

    FILE *bDescriptor = fopen("b_matrix.dat", "r");
    if (!bDescriptor) {
        fprintf(stderr, "B matrix file isn't found, execution is impossible\n");
        deleteMatrix(&A);
        return 1;
    }
    Matrix B = createMatrix(columnsInA, columnsInB);
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

    FILE *file = fopen("c_matrix.dat", "r");
    if (file) {
        Matrix oneThreadCalculated = createMatrix(rowsInA, columnsInB);
        loadMatrixFromFile(&oneThreadCalculated, file);
        if (!areMatricesEqual(C, oneThreadCalculated)) {
            fprintf(stderr, "Result is incorrect!\n");
        }
        deleteMatrix(&oneThreadCalculated);
        fclose(file);
    } else {
        fprintf(stderr, "Result data file isn't found, validation is impossible. Please generate file with generate_data.sh script\n");
    }

    deleteMatrix(&A);
    deleteMatrix(&B);
    deleteMatrix(&C);

    return 0;
}
