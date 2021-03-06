#include <assert.h>
#include <stdlib.h>
#include "equation.h"


// TODO: change functions in the equation according to the problem

double f(double t, double x) {
    return t * x;
}

double phi(double x) {
    return x;
}

double psi(double t) {
    return t;
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

// Leap-frog method solver
struct solution solve(struct equation eq, double tau, double h) {
    assert(h > 0.0);
    assert(tau > 0.0);
    assert(eq.phi(0.0) == eq.psi(0.0));

    struct solution result;
    result.K = (unsigned)(eq.T / tau);
    result.M = (unsigned)(eq.X / h);

    result.t = (double*) calloc(result.K, sizeof(double));
    assert(result.t);
    result.x = (double*) calloc(result.M, sizeof(double));
    assert(result.x);
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
        result.values[result.M + i] = result.values[i] + tau * (eq.f(tau, i * h) - 0.5 * eq.a / h * (result.values[i + 1] - result.values[i - 1]) +
                                                                0.5 * eq.a * tau / h / h * (result.values[i + 1] - 2.0 * result.values[i] + result.values[i - 1]));
    }

    // After that the leap-frog method is implemented
    for (int i = 2; i < result.M; ++i) {
        for (int j = 1; j < result.K; ++j) {
            result.values[result.M * i + j] = result.values[result.M * (i - 2) + j] + tau * (2.0 * eq.f((i - 1) * tau, j * h) -
                                                                                             eq.a / h * (result.values[result.M * (i - 1) + j + 1] -
                                                                                             result.values[result.M * (i - 1) + j - 1]));
        }
    }

    return result;
}
