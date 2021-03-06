#pragma once


typedef double (*func)(double, double);
typedef double (*cond)(double);

struct equation {
    double T;
    double X;
    double a;
    func f;
    cond phi;
    cond psi;
};

struct solution {
    double* values;
    double* t;
    double* x;
    unsigned int K;
    unsigned int M;
};

struct equation init();
struct solution solve(struct equation, double, double);
