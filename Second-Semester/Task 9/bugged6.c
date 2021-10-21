/******************************************************************************
* ЗАДАНИЕ: bugged6.c
* ОПИСАНИЕ:
*   Множественные ошибки компиляции
******************************************************************************/

#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

#define VECLEN 100

float a[VECLEN], b[VECLEN];

float dotprod()
{
    int i, tid;
    float sum;

    // if parallel section isn't moved there, sum
    // will be private
    #pragma omp parallel shared(sum)
    {
        #pragma omp for private(i, tid) reduction(+: sum)
        for (i = 0; i < VECLEN; i++)
        {
            tid = omp_get_thread_num();
            sum = sum + (a[i] * b[i]);
            printf("  tid= %d i=%d\n", tid, i);
        }
    }
    return sum;
}


int main (int argc, char *argv[])
{
    int i;
    float sum;

    for (i = 0; i < VECLEN; i++)
        a[i] = b[i] = 1.0 * i;
    // #pragma omp parallel shared(sum)
    // Why should one pass by value a variable into another function in this case?
    sum = dotprod();

    printf("Sum = %f\n",sum);
    return 0;
}
