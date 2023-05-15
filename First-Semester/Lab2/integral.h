#pragma once

struct pthread_data {
    int start_index;
    long int start_number;
    int end_index;
    long int end_number;
    double a;
};

typedef double (*function_type)(double);

double function(double x);

// Calculates n-points Simpson integral estimation
// a (double)   - left border of the integral
// n (long int) - number of steps
// h (double)   - step between adjacent points multiplied by two
// f (func)     - integrated function
// returns      - integral estimation
double simpsons_integration(double a, long int n, double h, function_type f);

void calculate_singlethreaded(double a, double b, double h);

// Calculates integral using CHILD_THREADS_NUM threads.
// This function breaks integration area [`a`, `b`] into `domains_num` domains, on each
// of which the step between points is constant.
// Between domains step grows as geometric progression. All parameters are
// calculated so that cumulative error does not exceed `epsilon`
void calculate_multithreaded(double a, double b, double epsilon, int domains_num);

void calculate(double a, double b, double epsilon, int domains_num, double factor);
