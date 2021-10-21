/******************************************************************************
* ЗАДАНИЕ: bugged2.c
* ОПИСАНИЕ:
*   Еще одна программа на OpenMP с багом. 
******************************************************************************/

#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
    int nthreads, i, tid;
    // float isn't sufficient for calculations:
    //      float             double
    // 3999522881536.0 vs 3999996000000.0 - results comparison
    double total;

    // `tid`, `i` and `total` variables must be private,
    // otherwise the race condition on `total` will appear.
    // Moreover, declaration of `total` as reduction variable must be
    // moved from inside the parallel section (line 33) to its definition
    #pragma omp parallel private(tid, i) reduction(+: total)
    {
        tid = omp_get_thread_num();
        if (tid == 0)
        {
            nthreads = omp_get_num_threads();
            printf("Number of threads = %d\n", nthreads);
        }
        printf("Thread %d is starting...\n", tid);

        #pragma omp barrier

        total = 0.0;
        // #pragma omp for schedule(dynamic, 10)
        // redundant because located in parallel section
        for (i = 0; i < 1000000; i++) {
            total = total + i * 1.0;
        }
        printf ("Thread %d is done! Total = %e\n", tid, total);
    }
    // Only after the end of parallel section
    // values from threads are accumulated into `total` variable
    printf("The result total = %.20f\n", total);
}
