// #include <assert.h>
// #include <math.h>
// #include "pthread.h"
// #include <stdio.h>
// #include <stdlib.h>
// #include <sys/time.h>
#include "integral.h"
#include "utils.h"

// #define CHILD_THREADS_NUM 7


// static pthread_mutex_t LOCK;
// double* RESULT = NULL;
// // Variables for integration grid settings
// long int* POINTS_PER_DOMAIN = NULL;
// double H0 = 0.0;
// double FACTOR = 0.0;


// struct pthread_data {
//     int start_index;
//     long int start_number;
//     int end_index;
//     long int end_number;
//     double a;
// };


// typedef double (*func)(double);

// double function(double x) {
//     assert(x != 0.0);
//     return sin(1.0 / x);
// }


// // Calculates n-points Simpson integral estimation
// // a (double)   - left border of the integral
// // n (long int) - number of steps
// // h (double)   - step between adjacent points multiplied by two
// // f (func)     - integrated function
// // returns      - integral estimation
// double simpsons_integration(double a, long int n, double h, func f) {
//     double res = h * f(a) / 6.0;
//     for (long int i = 1; i < n - 1; ++i) {
//         res += 2.0 / 3.0 * h * f(a + 0.5 * h);
//         a += h;
//         res += h * f(a) / 3.0;
//     }
//     res += 2.0 / 3.0 * h * f(a + 0.5 * h);
//     return res + h * f(a + h) / 6.0;
// }


// void thread_calculate_fixed_h(double a, long int n, double h, func f) {
//     double partial = simpsons_integration(a, n, h, f);
//     pthread_mutex_lock(&LOCK);
//     *RESULT += partial;
//     pthread_mutex_unlock(&LOCK);
// }


// void* child_thread_function(void* th_data) {
//     assert(th_data);
//     struct pthread_data* data = (struct pthread_data*)th_data;

//     int start_index = data->start_index;
//     long int start_number = data->start_number;
//     int end_index = data->end_index;
//     long int end_number = POINTS_PER_DOMAIN[start_index];

//     double a = data->a;
//     double h = H0 * pow_int(FACTOR, start_index);
//     if (start_index == end_index) {
//         // thread calculates within one domain
//         thread_calculate_fixed_h(a, data->end_number - start_number + 1, h, function);
//     } else {
//         // thread calculates across domains
//         double partial = simpsons_integration(a, end_number - start_number + 1, h, function);
//         a += h * (end_number - start_number + 1);
//         h *= FACTOR;
//         for (int i = start_index + 1; i < end_index; ++i) {
//             end_number = POINTS_PER_DOMAIN[i];
//             partial += simpsons_integration(a, end_number + 1, h, function);

//             a += h * (end_number + 1);
//             h *= FACTOR;
//         }

//         partial += simpsons_integration(a, data->end_number + 1, h, function);
//         pthread_mutex_lock(&LOCK);
//         *RESULT += partial;
//         pthread_mutex_unlock(&LOCK);
//     }
// }


// // Calculates initial integration step H0. For parallel algorithm
// // the step h is increasing during integration. Formula for H0
// // is derived with respect to the used Simpson's integration rule.
// double calculate_h0(double a, double b, double epsilon, double factor, int domains_num) {
//     double tmp = 0.0;
//     double x = a;
//     for (int i = 0; i < domains_num; ++i) {
//         // f(x) = sin(1/x) => max|f^(4)(x)| = 1/a^8
//         tmp += pow(factor, 4.5 * i) / pow(x + a * (sqrt(factor) - 1.0) * pow(factor, 0.5 * i), 8.0);
//     }

//     return fmin(pow(2880.0 * epsilon / a / (sqrt(factor) - 1.0) / tmp, 0.25), 0.01 * epsilon);
// }


// long int get_points_per_thread(int domains_num, double a, double h, double factor) {
//     assert(POINTS_PER_DOMAIN);

//     const double numerator = sqrt(factor) - 1.0;
//     const double e = h / a / a;
//     long int cumulative_n = 0;
//     for (int i = 0; i < domains_num; ++i) {
//         POINTS_PER_DOMAIN[i] = (long int)(numerator / sqrt(e * h)) + 1;
//         cumulative_n += POINTS_PER_DOMAIN[i];
//         h *= factor;
//     }
//     return cumulative_n / (CHILD_THREADS_NUM + 1) + 1;
// }


// void calculate_in_one_thread(double a, double b, double h) {
//     struct timeval begin, end;
//     gettimeofday(&begin, 0);
//     printf("By one-thread algorithm: I = %.20f\n", simpsons_integration(0.001, (long int)((b - a) / h) + 1, h, function));
//     gettimeofday(&end, 0);
//     printf("Time elapsed by one-thread algorithm: %.20f s\n", get_elapsed_time(begin, end));
// }


// // Integration part done in the main thread
// void calculate_main_thread(double a, double h,
//                            int domains_num, int domain_idx, long int domain_covered_num) {
//     // calculate remaining points in domain
//     double partial = simpsons_integration(
//         a, POINTS_PER_DOMAIN[domain_idx] - domain_covered_num + 1, h, function);
//     a += h * (POINTS_PER_DOMAIN[domain_idx] - domain_covered_num);
//     h *= FACTOR;

//     // calculate in remaining domains
//     for (int i = domain_idx + 1; i < domains_num; ++i) {
//         partial += simpsons_integration(a, POINTS_PER_DOMAIN[i], h, function);
//         a += h * POINTS_PER_DOMAIN[i];
//         h *= FACTOR;
//     }

//     pthread_mutex_lock(&LOCK);
//     *RESULT += partial;
//     pthread_mutex_unlock(&LOCK);
// }


// // Calculates integral using CHILD_THREADS_NUM threads.
// // This function breaks integration area [`a`, `b`] into `domains_num` domains, on each
// // of which the step between points is constant.
// // Between domains step grows as geometric progression. All parameters are
// // calculated so that cumulative error does not exceed `epsilon`
// void calculate_multithreaded(double a, double b, double epsilon, int domains_num) {
//     pthread_mutex_init(&LOCK, NULL);
//     pthread_t threads[CHILD_THREADS_NUM];
//     struct pthread_data threads_data[CHILD_THREADS_NUM];

//     POINTS_PER_DOMAIN = (long int*) calloc(domains_num, sizeof(long int));
//     assert(POINTS_PER_DOMAIN);
//     RESULT = (double*) calloc(1, sizeof(double));
//     assert(RESULT);

//     long int points_per_thread = get_points_per_thread(domains_num, a, H0, FACTOR);

//     struct timeval parallel_begin, parallel_end;
//     gettimeofday(&parallel_begin, 0);

//     int domain_idx = 0;
//     long int domain_covered_num = 0;
//     double h = H0;

//     // create child threads
//     for (int i = 0; i < CHILD_THREADS_NUM; ++i) {
//         long int points = 0;

//         double delta = 0.0;

//         threads_data[i].start_index = domain_idx;
//         threads_data[i].start_number = domain_covered_num;
//         threads_data[i].end_index = domain_idx;

//         for (; domain_idx < domains_num; ++domain_idx) {
//             if (points - domain_covered_num + POINTS_PER_DOMAIN[domain_idx] > points_per_thread) {
//                 // current domain contain enough points for one thread
//                 domain_covered_num += points_per_thread - points;

//                 threads_data[i].end_index = domain_idx;
//                 threads_data[i].end_number = domain_covered_num - 1;
//                 threads_data[i].a = a;

//                 pthread_create(&threads[i], NULL, child_thread_function, &threads_data[i]);

//                 a += delta + h * (points_per_thread - points);
//                 break;
//             } else {
//                 // proceed to the next domain
//                 long int domain_remaining_points = POINTS_PER_DOMAIN[domain_idx] - domain_covered_num;
//                 delta += h * (domain_remaining_points);
//                 points += domain_remaining_points;

//                 domain_covered_num = 0;
//                 h *= FACTOR;
//             }
//         }
//     }

//     // calculate in this thread
//     calculate_main_thread(a, h, domains_num, domain_idx, domain_covered_num);

//     // join threads and print results
//     struct timeval begin, end;
//     gettimeofday(&begin, 0);
//     for (int i = 0; i < CHILD_THREADS_NUM; ++i) {
//         pthread_join(threads[i], NULL);
//     }
//     gettimeofday(&end, 0);
//     gettimeofday(&parallel_end, 0);
//     print_result(*RESULT, parallel_begin, parallel_end, begin, end);

//     // release resources
//     free(RESULT);
//     free(POINTS_PER_DOMAIN);
//     pthread_mutex_destroy(&LOCK);
// }


// int main(int argc, char** argv) {
//     const double epsilon = get_epsilon(argc, argv);
//     double a = 0.001;
//     double b = 1.0;

//     FACTOR = 100.0;
//     // This value is calculated by formula m = 2 * log(b/a) / log(FACTOR)
//     const int domains_num = 3;

//     H0 = calculate_h0(a, b, epsilon, FACTOR, domains_num);

//     if (is_one_thread_mode()) {
//         calculate_in_one_thread(a, b, H0);
//     } else {
//         calculate_multithreaded(a, b, epsilon, domains_num);
//     }

//     return 0;
// }


int main(int argc, char** argv) {
    double epsilon = get_epsilon(argc, argv);
    double a = 0.001;
    double b = 1.0;
    double factor = 100.0;
    // This value is calculated by formula m = 2 * log(b/a) / log(FACTOR)
    int domains_num = 3;

    calculate(a, b, epsilon, domains_num, factor);

    return 0;
}
