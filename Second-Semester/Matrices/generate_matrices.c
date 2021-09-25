#include "matrix.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>


int main(int argc, char **argv) {
    if (argc != 4) {
        fprintf(stderr, "Dimensions of matrices are provided incorrectly; got %d arguments\n", argc - 1);
        exit(1);
    }

    srand((unsigned int)time(NULL) / 2);

    int rowsInA = atoi(argv[1]);
    if (rowsInA <= 0) {
        fprintf(stderr, "Incorrect value was used as number of rows in A; got %d <= 0\n", rowsInA);
        exit(1);
    }
    int columnsInA = atoi(argv[2]);
    if (columnsInA <= 0) {
        fprintf(stderr, "Incorrect value was used as number of columns in A; got %d <= 0\n", columnsInA);
        exit(1);
    }
    int columnsInB = atoi(argv[3]);
    if (columnsInB <= 0) {
        fprintf(stderr, "Incorrect value was used as number of columns in B; got %d <= 0\n", columnsInB);
        exit(1);
    }

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
