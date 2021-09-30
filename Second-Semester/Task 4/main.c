#include <omp.h>
#include "matrix.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>


int main(int argc, char **argv) {
    if (argc != 4) {
        fprintf(stderr, "Dimensions of matrices are provided incorrectly; got %d arguments\n", argc - 1);
        exit(1);
    }

    // Initialize dimensions of the matrices
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

    // Read A from file and init structure
    FILE *aDescriptor = fopen("a_matrix.dat", "r");
    if (!aDescriptor) {
        fprintf(stderr, "A matrix file isn't found, execution is impossible\n");
        return 1;
    }
    Matrix A = createMatrix(rowsInA, columnsInA);
    loadMatrixFromFile(&A, aDescriptor);
    fclose(aDescriptor);

    // Read B from file and init structure
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

    // Staff variables for loop
    int rowIndex = 0;
    int columnIndex = 0;
    int newElement = 0;
    int i = 0;

    // time measuring
    struct timeval begin, end;
    gettimeofday(&begin, 0);

    // collapse(2) will force omp to parallelize 2 nested loops,
    // otherwise only the outer loop will be parallelized.
    // This optimizes runs with small value of C.numberOfRows
    #pragma omp parallel for collapse(2) schedule(dynamic, 250) shared(A, B, C) private(rowIndex, columnIndex, newElement, i) default(none)
    for (rowIndex = 0; rowIndex < C.numberOfRows; ++rowIndex) {
        for (columnIndex = 0; columnIndex < C.numberOfColumns; ++columnIndex) {
            newElement = 0;
            for (i = 0; i < A.numberOfColumns; ++i) {
                newElement += getMatrixElement(&A, rowIndex, i) * getMatrixElement(&B, i, columnIndex);
            }
            setMatrixElement(&C, rowIndex, columnIndex, newElement);
        }
    }

    // time measuring
    gettimeofday(&end, 0);
    long seconds = end.tv_sec - begin.tv_sec;
    long microseconds = end.tv_usec - begin.tv_usec;
    double elapsed = seconds + microseconds * 1e-6;
    printf("Elapsed time equals %.20fs\n", elapsed);

    // validation part
    FILE *file = fopen("c_matrix.dat", "r");
    if (file) {
        Matrix oneThreadCalculated = createMatrix(rowsInA, columnsInB);
        loadMatrixFromFile(&oneThreadCalculated, file);
        if (areMatricesEqual(C, oneThreadCalculated) == 0) {
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
