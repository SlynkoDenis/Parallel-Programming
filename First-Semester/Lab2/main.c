#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include "pthread.h"


static pthread_mutex_t lock;
double* result = NULL;
// Variables for integration grid settings
long int* n = NULL;
double h0 = 0.0;
double s = 0.0;


struct pthread_data {
    int start_index;
    long int start_number;
    int end_index;
    long int end_number;
    double a;
};


typedef double (*func)(double);

double function(double x) {
    assert(x != 0.0);
    return sin(1.0 / x);
}


double simpsons_integration(double a, long int n, double h, func f) {
    double res = h * f(a) / 6.0;
    for (long int i = 1; i < n - 1; ++i) {
        res += 2.0 / 3.0 * h * f(a + 0.5 * h);
        a += h;
        res += h * f(a) / 3.0;
    }
    res += 2.0 / 3.0 * h * f(a + 0.5 * h);
    return res + h * f(a + h) / 6.0;
}


void* thread_function(void* th_data) {
    assert(th_data);
    struct pthread_data* data = (struct pthread_data*)th_data;

    int start_index = data->start_index;
    long int start_number = data->start_number;
    int end_index = data->end_index;
    long int end_number = n[start_index];

    double a = data->a;
    double h = h0 * pow(s, start_index);
    if (start_index == end_index) {
        end_number = data->end_number;

        pthread_mutex_lock(&lock);
        *result += simpsons_integration(a, end_number - start_number + 1, h, function);
        pthread_mutex_unlock(&lock);
    } else {
        double tmp = simpsons_integration(a, end_number - start_number + 1, h, function);
        a += h * (end_number - start_number + 1);
        h *= s;
        for (int i = start_index + 1; i < end_index; ++i) {
            end_number = n[i];
            tmp += simpsons_integration(a, end_number + 1, h, function);

            a += h * (end_number + 1);
            h *= s;
        }

        pthread_mutex_lock(&lock);
        *result += tmp + simpsons_integration(a, data->end_number + 1, h, function);
        pthread_mutex_unlock(&lock);
    }
}


double calculate_h0(double a, double b, double epsilon, double s, int m) {
    /* Calculates initial integration step h0. For parallel algorithm
    the step h is increasing during integration. Formula for h0
    is derived with respect to the used Simpson's integration rule. */
    double tmp = 0.0;
    double x = a;
    for (int i = 0; i < m; ++i) {
        tmp += pow(s, 4.5 * i) / pow(x + a * (sqrt(s) - 1.0) * pow(s, 0.5 * i), 8.0);
    }

    return fmin(pow(2880.0 * epsilon / a / (sqrt(s) - 1.0) / tmp, 0.25), 0.01 * epsilon);
}


double get_elapsed_time(struct timeval begin, struct timeval end) {
    /* Returns time in range expressed in seconds */
    long int seconds = end.tv_sec - begin.tv_sec;
    long int microseconds = end.tv_usec - begin.tv_usec;
    return seconds + microseconds * 1.0e-6;
}


int main(int argc, char** argv) {
    if (argc == 1) {
        fprintf(stderr, "Epsilon is unspecified\n");
        exit(1);
    }
    const double epsilon = atof(argv[1]);
    if (epsilon <= 0.0) {
        fprintf(stderr, "Invalid epsilon in input\n");
        exit(1);
    }
    double a = 0.001;
    double b = 1.0;

    s = 100.0;
    // This value is calculated by formula m = 2 * log(b / a) / log(s)
    const int m = 3;

    h0 = calculate_h0(a, b, epsilon, s, m);
    const double e = h0 / a / a;

    char* mode = getenv("MODE");
    if (mode && atoi(mode) == 0) {
        struct timeval begin, end;
        gettimeofday(&begin, 0);
        printf("By one-thread algorithm: I = %.20f\n", simpsons_integration(0.001, (long int)((b - 0.001) / h0) + 1, h0, function));
        gettimeofday(&end, 0);
        printf("Time elapsed by one-thread algorithm: %.20f s\n", get_elapsed_time(begin, end));

        return 0;
    }

    pthread_mutex_init(&lock, NULL);
    const unsigned int number_of_threads = 8;
    pthread_t* threads = (pthread_t*) calloc(number_of_threads - 1, sizeof(pthread_t));
    assert(threads);
    struct pthread_data* data = (struct pthread_data*) calloc(number_of_threads - 1, sizeof(struct pthread_data));
    assert(data);

    n = (long int*) calloc(m, sizeof(long int));
    assert(n);

    double h = h0;
    long int cumulative_n = 0;
    for (int i = 0; i < m; ++i) {
        n[i] = (long int)((pow(s, 0.5) - 1.0) / pow(e * h, 0.5)) + 1;
        cumulative_n += n[i];
        h *= s;
    }
    long int points_for_thread = cumulative_n / number_of_threads + 1;

    result = (double*) calloc(1, sizeof(double));
    assert(result);

    struct timeval parallel_begin, parallel_end;
    gettimeofday(&parallel_begin, 0);
    int j = 0;
    long int current_number = 0;
    h = h0;
    for (int i = 0; i < number_of_threads - 1; ++i) {
        long int points = 0;

        double delta = 0.0;

        data[i].start_index = j;
        data[i].start_number = current_number;
        data[i].end_index = j;

        for (; j < m; ++j) {
            if (points - current_number + n[j] > points_for_thread) {
                current_number += points_for_thread - points;

                data[i].end_index = j;
                data[i].end_number = current_number - 1;
                data[i].a = a;
                pthread_create(&threads[i], NULL, thread_function, &data[i]);

                a += delta + h * (points_for_thread - points);
                break;
            } else {
                delta += h * (n[j] - current_number);
                points += n[j] - current_number;

                cumulative_n -= n[j] - current_number;

                current_number = 0;
                h *= s;
            }
        }
    }

    // Integration part, computed in the main thread
    double tmp = simpsons_integration(a, n[j] - current_number + 1, h, function);
    a += h * (n[j] - current_number);
    h *= s;
    for (int i = j + 1; i < m; ++i) {
        tmp += simpsons_integration(a, n[i], h, function);
        a += h * n[i];
        h *= s;
    }
    pthread_mutex_lock(&lock);
    *result += tmp;
    pthread_mutex_unlock(&lock);


    struct timeval begin, end;
    gettimeofday(&begin, 0);
    for (int i = 0; i < number_of_threads - 1; ++i) {
        pthread_join(threads[i], NULL);
    }
    gettimeofday(&end, 0);
    gettimeofday(&parallel_end, 0);
    double elapsed = get_elapsed_time(begin, end);
    printf("Time delay for pthread_join(): %.20f s\n", elapsed);
    elapsed = get_elapsed_time(parallel_begin, parallel_end);
    printf("By parallel algorithm: I = %.20f\n", *result);
    printf("Time elapsed by parallel algorithm: %.20f s\n", elapsed);

    free(result);
    free(n);
    free(data);
    free(threads);
    pthread_mutex_destroy(&lock);
    return 0;
}
