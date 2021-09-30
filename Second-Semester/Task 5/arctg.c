#include <assert.h>
#include <math.h>
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>


static double calculate_arctg_coefficient(int n) {
    // C_n = (-1)^(n - 1) / (2n - 1)
    if (n < 1) {
        return 0.0;
    }

    double res = 2.0 * n - 1.0;

    if (res == INFINITY) {
        return 0.0;
    }
    return n % 2 == 0 ? -1.0 / res : 1.0 / res;
}


int main(int argc, char **argv) {
    // Get number of terms
    int number_of_terms = 0;
    if (argc == 1) {
        fprintf(stderr, "Number of terms wasn't provided; using default value 1000\n");
        number_of_terms = 1000;
    } else {
        number_of_terms = atoi(argv[1]);
        if (number_of_terms < 1) {
            fprintf(stderr, "Invalid value from stdin: %d < 1\n", number_of_terms);
            number_of_terms = 1000;
        }
    }
    // By default we compute arctg(1)
    double x = 1.0;    
    if (argc == 3) {
        x = atof(argv[2]);
        if (x < -1.0 || x > 1.0) {
            fprintf(stderr, "Invalid value from stdin: |%.10f| > 1\n", x);
            x = 1.0;
        }
    } else {
        fprintf(stderr, "Point to calculate in wasn't provided; using default value 1.0\n");
        x = 1.0;
    }

    // Counter and result variable
    int i = 0;
    double sum = 0.0;

    // Firstly we compute coefficients for all terms;
    // guided schedule will speed up computations
    double *coefficients = (double *) calloc(number_of_terms, sizeof(double));
    assert(coefficients);
    #pragma omp parallel for schedule(guided) private(i) shared(coefficients, number_of_terms, x) default(none)
    for (i = 1; i <= number_of_terms; ++i) {
        coefficients[i - 1] = calculate_arctg_coefficient(i) * pow(x, 2 * i - 1);
    }

    // Reduction to get the result
    #pragma omp parallel for private(i) shared(number_of_terms, coefficients) reduction(+: sum) default(none)
    for (i = 0; i < number_of_terms; ++i) {
        sum += coefficients[i];
    }

    printf("Calculated arctg(%.5f) = %.20f with %d iterations\n", x, sum, number_of_terms);
    free(coefficients);
    return 0;
}
