/******************************************************************************
* ЗАДАНИЕ: bugged4.c
* ОПИСАНИЕ:
*   Очень простая программа с segmentation fault.
******************************************************************************/

#include <assert.h>
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

// Original size is 1048, but with N > 1023 SEGFAULT occurs
// because of the stack overflow. Two solutions: either reduce N to <= 1023
// or allocate memory in heap rather than on stack. For instance,
// in this solution the arrays are allocated in heap.
#define N 1048

int main (int argc, char *argv[]) 
{
    int nthreads, tid, i, j;
    // double a[N][N];
    double **a = NULL;

    #pragma omp parallel shared(nthreads) private(i, j, tid, a)
    {
        // must allocate inside parallel section, because if pointers are created outside,
        // the compiler won't create copy of the raw pointers due to unknown (for compiler) size
        double **a = (double **) calloc(N, sizeof(double *));
        assert(a);
        for (i = 0; i < N; ++i) {
            a[i] = (double *) calloc(N, sizeof(double));
            assert(a[i]);
        }

        tid = omp_get_thread_num();
        if (tid == 0) 
        {
            nthreads = omp_get_num_threads();
            printf("Number of threads = %d\n", nthreads);
        }
        printf("Thread %d starting...\n", tid);

        #pragma omp barrier

        for (i = 0; i < N; i++)
            for (j = 0; j < N; j++) {
                a[i][j] = tid + i + j;
            }

        printf("Thread %d done. Last element= %f\n", tid, a[N-1][N-1]);

        // free
        for (i = 0; i < N; ++i) {
            free(a[i]);
        }
        free(a);
    } 
}
