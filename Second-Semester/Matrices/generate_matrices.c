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
    initMatrixWithRandomValues(&A);
    Matrix B = createMatrix(columnsInA, columnsInB);
    initMatrixWithRandomValues(&B);

    const char *nameOfA = "a_matrix.dat";
    const char *nameOfB = "b_matrix.dat";

    FILE *aDescriptor = fopen(nameOfA, "w");
    FILE *bDescriptor = fopen(nameOfB, "w");

    outputMatrixInFile(A, aDescriptor);
    outputMatrixInFile(B, bDescriptor);

    fclose(aDescriptor);
    fclose(bDescriptor);

    Matrix C = multiplyMatrices(A, B);
    const char *nameOfC = "c_matrix.dat";
    FILE *cDescriptor = fopen(nameOfC, "w");
    outputMatrixInFile(C, cDescriptor);
    fclose(cDescriptor);

    deleteMatrix(&A);
    deleteMatrix(&B);
    deleteMatrix(&C);

    return 0;
}
