/******************************************************************************
* ЗАДАНИЕ: bugged3.c
* ОПИСАНИЕ:
*   Ошибка времени выполнения.
******************************************************************************/

#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

#define N 50

int main (int argc, char **argv) 
{
    int i, nthreads, tid, section;
    float a[N], b[N], c[N];
    void print_results(float array[N], int tid, int section);

    for (i = 0; i < N; i++)
        a[i] = b[i] = i * 1.0;

    #pragma omp parallel private(c, i, tid, section)
    {
        tid = omp_get_thread_num();
        if (tid == 0)
        {
            nthreads = omp_get_num_threads();
            printf("Number of threads = %d\n", nthreads);
        }

        #pragma omp barrier
        printf("Thread %d starting...\n", tid);
        #pragma omp barrier

        #pragma omp sections nowait
        {
            #pragma omp section
            {
                section = 1;
                for (i = 0; i < N; i++)
                    c[i] = a[i] * b[i];
                print_results(c, tid, section);
            }

            #pragma omp section
            {
                section = 2;
                for (i = 0; i < N; i++)
                    c[i] = a[i] + b[i];
                print_results(c, tid, section);
            }
        }

        #pragma omp barrier
        printf("Thread %d exiting...\n",tid);
    }
}


void print_results(float array[N], int tid, int section) 
{
    int i, j;

    j = 1;
    #pragma omp critical
    {
        printf("\nThread %d did section %d. The results are:\n", tid, section);
        for (i = 0; i < N; i++)
        {
            printf("%e  ", array[i]);
            j++;
            if (j == 6)
            {
                printf("\n");
                j = 1;
            }
        }
        printf("\n");
    }

    // #pragma omp barrier
    // the reason of locks: sections from lines 37 and 45. They are called
    // only for 2 threads, regardless of total number of threads. It is happening
    // because `omp sections nowait` inside `omp parallel` isn't applied to
    // every thread, but only once. This leads to deadlock in cases of OMP_NUM_THREADS > 2,
    // because then there are threads for which `print_results()` isn't called
    printf("Thread %d done and synchronized.\n", tid); 
}
