#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include "../equation.h"
#include "mpi.h"


#define SEND_TAG 2001

int main(int argc, char** argv) {
    double tau = 0.001;
    double h = 0.05;

    struct equation eq = init();
    assert(eq.phi(0.0) == eq.psi(0.0));
    struct solution result;

    MPI_Status status;
    int root_process = 0;
    int my_id = 0;
    int num_procs = 0;
    int ierr = MPI_Init(&argc, &argv);
    ierr = MPI_Comm_rank(MPI_COMM_WORLD, &my_id);
    ierr = MPI_Comm_size(MPI_COMM_WORLD, &num_procs);

    int points_per_process = (int)(eq.X / h) / num_procs;
    assert(points_per_process > 3);

    result.K = (unsigned)(eq.T / tau) + 1;
    result.M = (unsigned)(points_per_process * num_procs);
    result.values = (double*) calloc(result.K * points_per_process, sizeof(double));
    assert(result.values);

    if (my_id == root_process) {
        struct timeval begin, end;
        gettimeofday(&begin, 0);

        double right_corner = eq.phi(points_per_process * h);
        for (int i = 0; i < points_per_process; ++i) {
            result.values[i] = eq.phi(i * h);
        }
        for (int i = 1; i < result.K; ++i) {
            result.values[i * points_per_process] = eq.psi(i * tau);
        }

        MPI_Request requests[2];

        // First time step must be done with a scheme other than leap-frog
        for (int i = 1; i < points_per_process - 1; ++i) {
            result.values[points_per_process + i] = result.values[i] + tau * (eq.f(0.0, i * h) - 0.5 * eq.a / h * (result.values[i + 1] - result.values[i - 1]) +
                                                                              0.5 * eq.a * tau / h / h * (result.values[i + 1] - 2.0 * result.values[i] + result.values[i - 1]));
        }
        result.values[2 * points_per_process - 1] = result.values[points_per_process - 1] +
                                                    tau * (eq.f(0.0, (points_per_process - 1) * h) - 0.5 * eq.a / h * (right_corner - result.values[points_per_process - 2]) +
                                                           0.5 * eq.a * tau / h / h * (right_corner - 2.0 * result.values[points_per_process - 1] +
                                                                                       result.values[points_per_process - 2]));
        MPI_Isend(result.values + 2 * points_per_process - 1, 1, MPI_DOUBLE, root_process + 1, SEND_TAG, MPI_COMM_WORLD, requests + 1);

        // After that the leap-frog method is implemented
        for (int i = 2; i < result.K; ++i) {
            MPI_Irecv(&right_corner, 1, MPI_DOUBLE, root_process + 1, MPI_ANY_TAG, MPI_COMM_WORLD, requests);

            int j = 1;
            for (; j < points_per_process - 1; ++j) {
                result.values[points_per_process * i + j] = result.values[points_per_process * (i - 2) + j] +
                                                            tau * (2.0 * eq.f((i - 1) * tau, j * h) -
                                                                   eq.a / h * (result.values[points_per_process * (i - 1) + j + 1] -
                                                                   result.values[points_per_process * (i - 1) + j - 1]));
            }
            
            MPI_Wait(requests, &status);
            result.values[points_per_process * i + j] = result.values[points_per_process * (i - 2) + j] +
                                                        tau * (2.0 * eq.f((i - 1) * tau, j * h) -
                                                               eq.a / h * (right_corner - result.values[points_per_process * (i - 1) + j - 1]));

            if (i != result.K - 1) {
                MPI_Isend(result.values + points_per_process * (i + 1) - 1, 1, MPI_DOUBLE, root_process + 1, SEND_TAG, MPI_COMM_WORLD, requests + 1);
            }
        }
        gettimeofday(&end, 0);
        long seconds = end.tv_sec - begin.tv_sec;
        long microseconds = end.tv_usec - begin.tv_usec;
        double elapsed = seconds + microseconds * 1e-6;
        printf("Solution time equals %.20f\n", elapsed);

        // Receive other parts of whole solution
        // NB! size of final_requests is big enough only for 4 processes
        assert(num_procs < 5);
        MPI_Request final_requests[4];

        int delta = result.K * points_per_process;
        double* other_parts = (double*) calloc(delta * (num_procs - 1), sizeof(double));
        assert(other_parts);
        for (int i = 1; i < num_procs; ++i) {
            MPI_Irecv(other_parts + (i - 1) * result.K * points_per_process, result.K * points_per_process, MPI_DOUBLE, i, MPI_ANY_TAG, MPI_COMM_WORLD, final_requests + i - 1);
        }
        MPI_Waitall(num_procs - 1, final_requests, MPI_STATUSES_IGNORE);

        FILE* fd = fopen("data.csv", "wb");
        assert(fd);
        for (unsigned int i = 0; i < result.K; ++i) {
            for (unsigned int j = 0; j < points_per_process; ++j) {
                fprintf(fd, "%.20f,", result.values[i * points_per_process + j]);
            }
            for (int k = 0, end_index = num_procs - 1; k < end_index; ++k) {
                for (unsigned int j = 0; j < points_per_process; ++j) {
                    fprintf(fd, "%.20f,", other_parts[i * points_per_process + j + k * delta]);
                }
            }
        }
        fprintf(fd, "%d,%.20f,%d,%.20f", (int)result.K, tau, (int)result.M, h);
        fclose(fd);
        free(other_parts);
    } else if (my_id < num_procs - 1) {
        int left_index = my_id * points_per_process;
        int right_index = (my_id + 1) * points_per_process;

        double left_corner = eq.phi((left_index - 1) * h);
        double right_corner = eq.phi(right_index * h);
        for (int i = left_index; i < right_index; ++i) {
            result.values[i - left_index] = eq.phi(i * h);
        }

        MPI_Request requests[4];
        MPI_Status stats[4];

        // First time step must be done with a scheme other than leap-frog
        result.values[points_per_process] = result.values[0] + tau * (eq.f(0.0, left_index * h) - 0.5 * eq.a / h * (result.values[1] - left_corner) +
                                                                      0.5 * eq.a * tau / h / h * (result.values[1] - 2.0 * result.values[0] + left_corner));
        for (int i = 1; i < points_per_process - 1; ++i) {
            result.values[points_per_process + i] = result.values[i] + tau * (eq.f(0.0, (i + left_index) * h) - 0.5 * eq.a / h * (result.values[i + 1] - result.values[i - 1]) +
                                                                              0.5 * eq.a * tau / h / h * (result.values[i + 1] - 2.0 * result.values[i] + result.values[i - 1]));
        }
        result.values[2 * points_per_process - 1] = result.values[points_per_process - 1] +
                                                    tau * (eq.f(0.0, (right_index - 1) * h) - 0.5 * eq.a / h * (right_corner - result.values[points_per_process - 2]) +
                                                           0.5 * eq.a * tau / h / h * (right_corner - 2.0 * result.values[points_per_process - 1] +
                                                                                       result.values[points_per_process - 2]));
        MPI_Isend(result.values + points_per_process, 1, MPI_DOUBLE, my_id - 1, SEND_TAG, MPI_COMM_WORLD, requests + 2);
        MPI_Isend(result.values + 2 * points_per_process - 1, 1, MPI_DOUBLE, my_id + 1, SEND_TAG, MPI_COMM_WORLD, requests + 3);

        // After that the leap-frog method is implemented
        for (int i = 2; i < result.K; ++i) {
            MPI_Irecv(&left_corner, 1, MPI_DOUBLE, my_id - 1, MPI_ANY_TAG, MPI_COMM_WORLD, requests);
            MPI_Irecv(&right_corner, 1, MPI_DOUBLE, my_id + 1, MPI_ANY_TAG, MPI_COMM_WORLD, requests + 1);

            int j = 1;
            for (; j < points_per_process - 1; ++j) {
                result.values[points_per_process * i + j] = result.values[points_per_process * (i - 2) + j] +
                                                            tau * (2.0 * eq.f((i - 1) * tau, (j + left_index) * h) -
                                                                   eq.a / h * (result.values[points_per_process * (i - 1) + j + 1] -
                                                                   result.values[points_per_process * (i - 1) + j - 1]));
            }
            
            MPI_Wait(requests, stats);
            result.values[points_per_process * i] = result.values[points_per_process * (i - 2)] +
                                                        tau * (2.0 * eq.f((i - 1) * tau, left_index * h) -
                                                               eq.a / h * (result.values[points_per_process * (i - 1) + 1] - left_corner));
            MPI_Wait(requests + 1, stats + 1);
            result.values[points_per_process * i + j] = result.values[points_per_process * (i - 2) + j] +
                                                        tau * (2.0 * eq.f((i - 1) * tau, (right_index - 1) * h) -
                                                               eq.a / h * (right_corner - result.values[points_per_process * (i - 1) + j - 1]));

            if (i != result.K - 1) {
                MPI_Isend(result.values + points_per_process * i, 1, MPI_DOUBLE, my_id - 1, SEND_TAG, MPI_COMM_WORLD, requests + 2);
                MPI_Isend(result.values + points_per_process * (i + 1) - 1, 1, MPI_DOUBLE, my_id + 1, SEND_TAG, MPI_COMM_WORLD, requests + 3);
            }
        }

        // Send part of solution to the root process
        MPI_Isend(result.values, result.K * points_per_process, MPI_DOUBLE, root_process, SEND_TAG, MPI_COMM_WORLD, requests);
        MPI_Wait(requests, &status);
    } else {
        int left_index = my_id * points_per_process;

        double left_corner = eq.phi((left_index - 1) * h);
        for (int i = left_index; i < result.M; ++i) {
            result.values[i - left_index] = eq.phi(i * h);
        }

        MPI_Request requests[2];

        // First time step must be done with a scheme other than leap-frog
        result.values[points_per_process] = result.values[0] + tau * (eq.f(0.0, left_index * h) - 0.5 * eq.a / h * (result.values[1] - left_corner) +
                                                                      0.5 * eq.a * tau / h / h * (result.values[1] - 2.0 * result.values[0] + left_corner));
        for (int i = 1; i < points_per_process - 1; ++i) {
            result.values[points_per_process + i] = result.values[i] + tau * (eq.f(0.0, (i + left_index) * h) - 0.5 * eq.a / h * (result.values[i + 1] - result.values[i - 1]) +
                                                                              0.5 * eq.a * tau / h / h * (result.values[i + 1] - 2.0 * result.values[i] + result.values[i - 1]));
        }
        result.values[2 * points_per_process - 1] = result.values[points_per_process - 1] + tau * (eq.f(0.0, (result.M - 1) * h) -
                                                    eq.a / h * (result.values[points_per_process - 1] - result.values[points_per_process - 2]));
        MPI_Isend(result.values + 2 * points_per_process - 1, 1, MPI_DOUBLE, my_id - 1, SEND_TAG, MPI_COMM_WORLD, requests + 1);

        // After that the leap-frog method is implemented
        for (int i = 2; i < result.K; ++i) {
            MPI_Irecv(&left_corner, 1, MPI_DOUBLE, my_id - 1, MPI_ANY_TAG, MPI_COMM_WORLD, requests);

            int j = 1;
            for (; j < points_per_process - 1; ++j) {
                result.values[points_per_process * i + j] = result.values[points_per_process * (i - 2) + j] +
                                                            tau * (2.0 * eq.f((i - 1) * tau, (j + left_index) * h) -
                                                                   eq.a / h * (result.values[points_per_process * (i - 1) + j + 1] -
                                                                   result.values[points_per_process * (i - 1) + j - 1]));
            }

            MPI_Wait(requests, &status);
            result.values[points_per_process * i] = result.values[points_per_process * (i - 2)] +
                                                        tau * (2.0 * eq.f((i - 1) * tau, left_index * h) -
                                                               eq.a / h * (result.values[points_per_process * (i - 1) + 1] - left_corner));
            result.values[points_per_process * i + j] = result.values[points_per_process * (i - 1) + j] +
                                                        tau * (eq.f((i - 1) * tau, (result.M - 1) * h) -
                                                               eq.a / h * (result.values[points_per_process * (i - 1) + j] -
                                                               result.values[points_per_process * (i - 1) + j - 1]));

            if (i != result.K - 1) {
                MPI_Isend(result.values + points_per_process * i, 1, MPI_DOUBLE, my_id - 1, SEND_TAG, MPI_COMM_WORLD, requests + 1);
            }
        }

        // Send part of solution to the root process
        MPI_Isend(result.values, result.K * points_per_process, MPI_DOUBLE, root_process, SEND_TAG, MPI_COMM_WORLD, requests);
        MPI_Wait(requests, &status);
    }

    delete(result);
    ierr = MPI_Finalize();
    return 0;
}
