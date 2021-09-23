#pragma once

#include <stdio.h>


typedef struct {
    int *elements;
    int numberOfRows;
    int numberOfColumns;
} Matrix;


extern Matrix createMatrix(int numberOfRows, int numberOfColumns);

extern void initMatrixWithRandomValues(Matrix *matrix);

extern void deleteMatrix(Matrix *matrix);

extern int setMatrixElement(Matrix *matrix, int row, int column, int new_element);

extern int getMatrixElement(Matrix *matrix, int row, int column);

extern int outputMatrixInFile(Matrix matrix, FILE *file);

extern Matrix multiplyMatrices(Matrix A, Matrix B);

extern void loadMatrixFromFile(Matrix *matrix, FILE *file);

extern int areMatricesEqual(Matrix lhs, Matrix rhs);
