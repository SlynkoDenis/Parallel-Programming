#include <omp.h>
#include <stdio.h>


int main(int argc, char **argv) {
    int max_threads = 0;
    int i = 0;

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
