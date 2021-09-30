#include <assert.h>
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>


static double calculate_factorial(int number) {
    double res = 1.0;
    for (int i = number; i > 1; --i) {
        res *= i;
    }

    return res;
}


int main(int argc, char **argv) {
    // Get number of terms
    int number_of_terms = 0;
    if (argc == 1) {
        fprintf(stderr, "Number of terms wasn't provided; using default value 100\n");
        number_of_terms = 100;
    } else {
        number_of_terms = atoi(argv[1]);
        if (number_of_terms < 1) {
            fprintf(stderr, "Invalid value from stdin: %d < 1\n", number_of_terms);
            number_of_terms = 100;
        }
    }

    // Counter and result variable
    int i = 0;
    double sum = 1.0;

    // Firstly we compute coefficients for all terms;
    // guided schedule will speed up computations
    double *factorials = (double *) calloc(number_of_terms, sizeof(double));
    assert(factorials);
    #pragma omp parallel for schedule(guided) private(i) shared(factorials, number_of_terms) default(none)
    for (i = 0; i < number_of_terms; ++i) {
        factorials[i] = calculate_factorial(i + 1);
    }

    // Reduction to get the result
    #pragma omp parallel for private(i) shared(number_of_terms, factorials) reduction(+: sum) default(none)
    for (i = 0; i < number_of_terms; ++i) {
        sum += 1.0 / factorials[i];
    }

    printf("Calculated e = %.20f with %d iterations\n", sum, number_of_terms);
    free(factorials);
    return 0;
}
