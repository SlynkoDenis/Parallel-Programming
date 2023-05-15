#include "assert.h"
#include "integral.h"
#include <math.h>
#include "pthread.h"
#include <stdio.h>
#include <stdlib.h>
#include "utils.h"


double function(double x) {
    assert(x != 0.0);
    return sin(1.0 / x);
}

double simpsons_integration(double a, long int n, double h, function_type f) {
    double res = h * f(a) / 6.0;
    for (long int i = 1; i < n - 1; ++i) {
        res += 2.0 / 3.0 * h * f(a + 0.5 * h);
        a += h;
        res += h * f(a) / 3.0;
    }
    res += 2.0 / 3.0 * h * f(a + 0.5 * h);
    return res + h * f(a + h) / 6.0;
}

void calculate_singlethreaded(double a, double b, double h) {
    struct timeval begin, end;
    gettimeofday(&begin, 0);
    printf("By one-thread algorithm: I = %.20f\n", simpsons_integration(0.001, (long int)((b - a) / h) + 1, h, function));
    gettimeofday(&end, 0);
    printf("Time elapsed by one-thread algorithm: %.20f s\n", get_elapsed_time(begin, end));
}
