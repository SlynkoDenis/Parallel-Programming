#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include "mpi.h"

#define ISIZE 1000
#define JSIZE 1000
#define SEND_TAG 2001


int main(int argc, char **argv) {
    double a[ISIZE][JSIZE];
    int i = 0;
    int j = 0;
    FILE *ff = NULL;
    for (i = 0; i < ISIZE; ++i){
        for (j = 0; j < JSIZE; ++j){
            a[i][j] = 10 * i + j;
        }
    }
    for (i = 0; i < ISIZE - 4; ++i) {
        for (j = 5; j < JSIZE; ++j){
            a[i][j] = sin(0.00001 * a[i + 4][j - 5]);
        }
    }

    ff = fopen("result.txt","w");
    for(i = 0; i < ISIZE; ++i) {
        for (j = 0; j < JSIZE; ++j){
            fprintf(ff, "%f ", a[i][j]);
        }
        fprintf(ff, "\n");
    }
    fclose(ff);

    MPI_Status status;
    int root_process = 0;
    int my_id = 0;
    int num_procs = 0;
    int ierr = MPI_Init(&argc, &argv);
    ierr = MPI_Comm_rank(MPI_COMM_WORLD, &my_id);
    ierr = MPI_Comm_size(MPI_COMM_WORLD, &num_procs);


    if (my_id == root_process) {
        struct timeval begin, end;
        gettimeofday(&begin, 0);

        MPI_Request requests[3];

        // First time step must be done with a scheme other than leap-frog
        for (int i = 1; i < points_per_process - 1; ++i) {
            result.values[points_per_process + i] = result.values[i] + tau * (eq.f(0.0, i * h) - 0.5 * eq.a / h * (result.values[i + 1] - result.values[i - 1]) +
                                                                              0.5 * eq.a * tau / h / h * (result.values[i + 1] - 2.0 * result.values[i] + result.values[i - 1]));
        }
        result.values[2 * points_per_process - 1] = result.values[points_per_process - 1] +
                                                    tau * (eq.f(0.0, (points_per_process - 1) * h) - 0.5 * eq.a / h * (right_corner - result.values[points_per_process - 2]) +
                                                           0.5 * eq.a * tau / h / h * (right_corner - 2.0 * result.values[points_per_process - 1] +
                                                                                       result.values[points_per_process - 2]));
        MPI_Isend(result.values + 2 * points_per_process - 1, 1, MPI_DOUBLE, root_process + 1, SEND_TAG, MPI_COMM_WORLD, requests + 2);

        assert(MPI_Recv_init(&right_corner, 1, MPI_DOUBLE, root_process + 1, MPI_ANY_TAG, MPI_COMM_WORLD,
                             requests) == MPI_SUCCESS);
        // Hack to make a template for sending the bounder value
        // In this var the value from the result.values array will be written
        double boundary_value = 0.0;
        assert(MPI_Send_init(&boundary_value, 1, MPI_DOUBLE, root_process + 1,
                             SEND_TAG, MPI_COMM_WORLD, requests + 1) == MPI_SUCCESS);

        // After that the leap-frog method is implemented
        for (int i = 2; i < result.K; ++i) {
            MPI_Start(requests);

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
                boundary_value = result.values[points_per_process * (i + 1) - 1];
                MPI_Wait(requests + 1, &status);
                MPI_Start(requests + 1);
            }
        }
        MPI_Request_free(requests);
        MPI_Request_free(requests + 1);

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

        MPI_Request requests[6];
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
        MPI_Isend(result.values + points_per_process, 1, MPI_DOUBLE, my_id - 1, SEND_TAG, MPI_COMM_WORLD, requests + 4);
        MPI_Isend(result.values + 2 * points_per_process - 1, 1, MPI_DOUBLE, my_id + 1, SEND_TAG, MPI_COMM_WORLD, requests + 5);

        assert(MPI_Recv_init(&left_corner, 1, MPI_DOUBLE, my_id - 1, MPI_ANY_TAG, MPI_COMM_WORLD, requests) == MPI_SUCCESS);
        assert(MPI_Recv_init(&right_corner, 1, MPI_DOUBLE, my_id + 1, MPI_ANY_TAG, MPI_COMM_WORLD, requests + 1) == MPI_SUCCESS);
        double boundary_values[2] = {0.0, 0.0};
        assert(MPI_Send_init(boundary_values, 1, MPI_DOUBLE, my_id - 1, SEND_TAG, MPI_COMM_WORLD, requests + 2) == MPI_SUCCESS);
        assert(MPI_Send_init(boundary_values + 1, 1, MPI_DOUBLE, my_id + 1, SEND_TAG,
                             MPI_COMM_WORLD, requests + 3) == MPI_SUCCESS);

        // After that the leap-frog method is implemented
        for (int i = 2; i < result.K; ++i) {
            MPI_Startall(2, requests);

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
                boundary_values[0] = result.values[points_per_process * i];
                boundary_values[1] = result.values[points_per_process * (i + 1) - 1];
                MPI_Waitall(2, requests + 2, stats + 2);
                MPI_Startall(2, requests + 2);
            }
        }
        for (int i = 0; i < 4; ++i) {
            MPI_Request_free(requests + i);
        }

        // Send part of solution to the root process
        MPI_Isend(result.values, result.K * points_per_process, MPI_DOUBLE, root_process, SEND_TAG, MPI_COMM_WORLD, requests + 4);
        MPI_Wait(requests + 4, &status);
    } else {
        int left_index = my_id * points_per_process;

        double left_corner = eq.phi((left_index - 1) * h);
        for (int i = left_index; i < result.M; ++i) {
            result.values[i - left_index] = eq.phi(i * h);
        }

        MPI_Request requests[3];

        // First time step must be done with a scheme other than leap-frog
        result.values[points_per_process] = result.values[0] + tau * (eq.f(0.0, left_index * h) - 0.5 * eq.a / h * (result.values[1] - left_corner) +
                                                                      0.5 * eq.a * tau / h / h * (result.values[1] - 2.0 * result.values[0] + left_corner));
        for (int i = 1; i < points_per_process - 1; ++i) {
            result.values[points_per_process + i] = result.values[i] + tau * (eq.f(0.0, (i + left_index) * h) - 0.5 * eq.a / h * (result.values[i + 1] - result.values[i - 1]) +
                                                                              0.5 * eq.a * tau / h / h * (result.values[i + 1] - 2.0 * result.values[i] + result.values[i - 1]));
        }
        result.values[2 * points_per_process - 1] = result.values[points_per_process - 1] + tau * (eq.f(0.0, (result.M - 1) * h) -
                                                    eq.a / h * (result.values[points_per_process - 1] - result.values[points_per_process - 2]));
        MPI_Isend(result.values + 2 * points_per_process - 1, 1, MPI_DOUBLE, my_id - 1, SEND_TAG, MPI_COMM_WORLD, requests + 2);

        assert(MPI_Recv_init(&left_corner, 1, MPI_DOUBLE, my_id - 1, MPI_ANY_TAG, MPI_COMM_WORLD, requests) == MPI_SUCCESS);
        double boundary_value = 0.0;
        assert(MPI_Send_init(&boundary_value, 1, MPI_DOUBLE, my_id - 1, SEND_TAG, MPI_COMM_WORLD, requests + 1) == MPI_SUCCESS);

        // After that the leap-frog method is implemented
        for (int i = 2; i < result.K; ++i) {
            MPI_Start(requests);

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
                boundary_value = result.values[points_per_process * i];
                MPI_Wait(requests + 1, &status);
                MPI_Start(requests + 1);
            }
        }
        MPI_Request_free(requests);
        MPI_Request_free(requests + 1);

        // Send part of solution to the root process
        MPI_Isend(result.values, result.K * points_per_process, MPI_DOUBLE, root_process, SEND_TAG, MPI_COMM_WORLD, requests + 2);
        MPI_Wait(requests + 2, &status);
    }

    ierr = MPI_Finalize();
    return 0;
}
