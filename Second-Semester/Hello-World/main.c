#include <omp.h>
#include <stdio.h>


int main(int argc, char **argv) {
    int max_threads = 0;
    int i = 0;

    /*
    #pragma omp parallel private(max_threads, i)
    {
        max_threads = omp_get_max_threads();
        #pragma omp for ordered
        for (i = 0; i < max_threads; ++i) {
            #pragma omp ordered
            {
                printf("Hello World from %d / %d!\n", omp_get_thread_num(), max_threads);
            }
        }
    }
    */

    #pragma omp parallel shared(i)
    while(1) {
        if (omp_get_thread_num() == omp_get_num_threads() - 1 - i) {
            printf("Hello World from %d / %d!\n", omp_get_thread_num(), omp_get_num_threads());        
            #pragma omp atomic
            ++i;
            break;
        }
    }
    return 0;
}
