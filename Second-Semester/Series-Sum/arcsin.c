#include <assert.h>
#include <math.h>
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


static double calculate_double_factorial(int start, int finish) {
    assert(finish > start);
    double res = 1.0;
    for (int i = start; i < finish; i += 2) {
        res *= i;
    }

    return res;
}


static double calculate_arcsin_coefficient(int n) {
    // C_n = (2n)! / (4^n * (n!)^2 * (2n + 1))
    if (n == 0) {
        return 1.0;
    }
    if (n == 1) {
        return 1.0 / 6.0;
    }

    double a = 0.0;
    double b = 0.0;
    double c = 0.0;
    if (n % 2 == 0) {
        a = calculate_double_factorial(n + 1, 2 * n);
        b = pow(2.0, (double)(2 * n - n / 2));
        c = calculate_factorial(n / 2) * (2 * n + 1);
    } else {
        a = calculate_double_factorial(n + 2, 2 * n);
        b = pow(2.0, (double)(2 * n - (n + 1) / 2));
        c = calculate_factorial((n - 1) / 2) * (2 * n + 1);
    }

    if (a == INFINITY || b == 0 || c == 0) {
        return 0.0;
    }
    return a / b / c;
}


int main(int argc, char **argv) {
    int number_of_terms = 0;
    if (argc == 1) {
        fprintf(stderr, "Number of terms wasn't provided; using default value 100\n");
        number_of_terms = 100;
    } else {
        number_of_terms = atoi(argv[1]);
        if (number_of_terms < 1) {
            fprintf(stderr, "Value from stdin: %d < 1\n", number_of_terms);
            number_of_terms = 100;
        }
    }
    int i = 0;
    double sum = 0.0;

    double *coefficients = (double *) calloc(number_of_terms, sizeof(double));
    assert(coefficients);
    #pragma omp parallel for private(i) shared(coefficients, number_of_terms) default(none)
    for (i = 0; i < number_of_terms; ++i) {
        coefficients[i] = calculate_arcsin_coefficient(i);
    }

    for (i = 0; i < number_of_terms; ++i) {
        printf("%d: %.20f\n", i, coefficients[i]);
    }

    #pragma omp parallel for private(i) shared(number_of_terms, coefficients) reduction(+: sum) default(none)
    for (i = 0; i < number_of_terms; ++i) {
        sum += 2.0 * coefficients[i];
    }

    printf("Calculated number %.20f with %d iterations\n", sum, number_of_terms);
    free(coefficients);
    return 0;
}
