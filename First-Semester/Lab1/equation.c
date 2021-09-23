#include <assert.h>
#include <stdlib.h>
#include "equation.h"


// TODO: change functions in the equation according to the problem

double f(double t, double x) {
    return x * t;
}


double phi(double x) {
    return x;
}


double psi(double t) {
    return -1.0 * t;
}


struct equation init() {
    struct equation result;
    result.T = 10.0;
    result.X = 10.0;
    result.a = 1.0;
    result.f = f;
    result.phi = phi;
    result.psi = psi;
    
    return result;
}


void delete(struct solution sol) {
    free(sol.values);
}


// Leap-frog method solver
struct solution solve(struct equation eq, double tau, double h) {
    assert(h > 0.0);
    assert(tau > 0.0);
    assert(eq.phi(0.0) == eq.psi(0.0));

    struct solution result;
    result.K = (unsigned)(eq.T / tau) + 1;
    result.M = (unsigned)(eq.X / h) + 1;

    result.values = (double*) calloc(result.K * result.M, sizeof(double));
    assert(result.values);

    for (int i = 0; i < result.M; ++i) {
        result.values[i] = eq.phi(i * h);
    }
    for (int i = 1; i < result.K; ++i) {
        result.values[i * result.M] = eq.psi(i * tau);
    }

    // First time step must be done with a scheme other than leap-frog
    for (int i = 1; i < result.M; ++i) {
        result.values[result.M + i] = result.values[i] + tau * (eq.f(0.0, i * h) - eq.a / h * (result.values[i] - result.values[i - 1]));
    }

    // After that the leap-frog method is implemented
    for (int i = 2; i < result.K; ++i) {
        int j = 1;
        for (; j < result.M - 1; ++j) {
            result.values[result.M * i + j] = result.values[result.M * (i - 2) + j] + tau * (2.0 * eq.f((i - 1) * tau, j * h) -
                                                                                             eq.a / h * (result.values[result.M * (i - 1) + j + 1] -
                                                                                             result.values[result.M * (i - 1) + j - 1]));
        }
        result.values[result.M * i + j] = result.values[result.M * (i - 1) + j] + tau * (eq.f((i - 1) * tau, j * h) -
                                                                                         eq.a / h * (result.values[result.M * (i - 1) + j] -
                                                                                         result.values[result.M * (i - 1) + j - 1]));
    }

    return result;
}
