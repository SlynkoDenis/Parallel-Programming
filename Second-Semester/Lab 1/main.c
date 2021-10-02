#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include "mpi.h"

#define ISIZE 1000
#define JSIZE 1000
#define SEND_TAG 2001
#define TERM 0.00001
#define INITIAL_TERM 10.0
#define EPS 1e-6

static int dequal(double lhs, double rhs) {
    return fabs(rhs - lhs) < EPS;
}


int main(int argc, char **argv) {
    const char *filename = "result.txt";
    double a[ISIZE][JSIZE];

    const int root_process = 0;
    int my_id = 0;
    int num_procs = 0;
    int ierr = MPI_Init(&argc, &argv);
    if (ierr != MPI_SUCCESS) {
        fprintf(stderr, "Failed to initialize MPI: %d\n", ierr);
        exit(ierr);
    }
    ierr = MPI_Comm_rank(MPI_COMM_WORLD, &my_id);
    ierr = MPI_Comm_size(MPI_COMM_WORLD, &num_procs);

    int delta = ISIZE / num_procs;

    if (my_id == root_process) {
        // time measurement
        struct timeval begin, end;
        gettimeofday(&begin, 0);

        for (int i = 0; i < delta; ++i) {
            // First 5 columns of the matrix are intact in relation to sin() loop
            for (int j = 0; j < 5; ++j) {
                a[i][j] = INITIAL_TERM * i + j;
            }

            for (int j = 5; j < JSIZE; ++j) {
                a[i][j] = sin(TERM * (INITIAL_TERM * (i + 4) + (j - 5)));
            }
        }

        // receive computed values from other nodes
        MPI_Request *requests = (MPI_Request *) calloc(num_procs - 1, sizeof(MPI_Request));
        assert(requests);

        for (int i = 1, end_index = num_procs - 1; i < end_index; ++i) {
            MPI_Irecv(*(a + i * delta), JSIZE * delta, MPI_DOUBLE, i,
                      MPI_ANY_TAG, MPI_COMM_WORLD, requests + i - 1);
        }
        MPI_Irecv(*(a + (num_procs - 1) * delta), JSIZE * (ISIZE - delta * (num_procs - 1)), MPI_DOUBLE,
                  num_procs - 1, MPI_ANY_TAG, MPI_COMM_WORLD, requests + num_procs - 2);
        MPI_Waitall(num_procs - 1, requests, MPI_STATUSES_IGNORE);

        // time measurement
        gettimeofday(&end, 0);
        long seconds = end.tv_sec - begin.tv_sec;
        long microseconds = end.tv_usec - begin.tv_usec;
        double elapsed = seconds + microseconds * 1e-6;
        printf("Time elapsed %.20fs\n", elapsed);

        // validation part
        FILE* fd = fopen(filename, "r");
        if (fd) {
            double tmp = 0.0;
            int error_occured = 0;
            for (int i = 0; i < ISIZE; ++i) {
                if (error_occured) {
                    break;
                }

                for (int j = 0; j < JSIZE; ++j) {
                    // if fail to read the next double
                    if (fscanf(fd, "%lf", &tmp) != 1) {
                        fprintf(stderr, "Failed to read element (%d, %d) from %s\n", i, j, filename);
                        error_occured = 1;
                        break;
                    }
                    if (!dequal(tmp, a[i][j])) {
                        fprintf(stderr, "Incorect value of the element (%d, %d): %.20f != %.20f\n", i, j, a[i][j], tmp);
                        error_occured = 1;
                        break;   
                    }
                }
            }
            fclose(fd);
            free(requests);
        } else {
            fprintf(stderr, "Failed to open file %s\n", filename);
        }
    } else {
        int start_index = delta * my_id;
        int end_index = 0;
        if (my_id == num_procs - 1) {
            // Last 4 rows of the matrix must be computed differently
            end_index = ISIZE - 8;
            for (int i = ISIZE - 4; i < ISIZE; ++i) {
                for (int j = 0; j < JSIZE; ++j) {
                    a[i][j] = INITIAL_TERM * i + j;
                }
            }

            for (int i = end_index, tmp_index = ISIZE - 4; i < tmp_index; ++i) {
                for (int j = 0; j < 5; ++j) {
                    a[i][j] = INITIAL_TERM * i + j;
                }

                // Little computations optimization
                for (int j = 5; j < JSIZE; ++j) {
                    a[i][j] = sin(TERM * a[i + 4][j - 5]);
                }
            }
        } else {
            end_index = start_index + delta;
        }

        for (int i = start_index; i < end_index; ++i) {
            // First 5 columns of the matrix are intact in relation to sin() loop
            for (int j = 0; j < 5; ++j) {
                a[i][j] = INITIAL_TERM * i + j;
            }

            for (int j = 5; j < JSIZE; ++j) {
                a[i][j] = sin(TERM * (INITIAL_TERM * (i + 4) + (j - 5)));
            }
        }

        int number_of_sent_elements = 0;
        if (my_id == num_procs - 1) {
            number_of_sent_elements = JSIZE * (ISIZE - start_index);
        } else {
            number_of_sent_elements = JSIZE * delta;
        }

        // send only computed values
        MPI_Send(*(a + start_index), number_of_sent_elements, MPI_DOUBLE, root_process, SEND_TAG, MPI_COMM_WORLD);
    }

    ierr = MPI_Finalize();
    return 0;
}
