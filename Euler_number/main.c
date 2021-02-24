#include <stdio.h>
#include "mpi.h"

#define SEND_TAG 2001

int main(int argc, char **argv) {
    const int num_of_iterations = 20;

    // variables for calculation on root process side
    double e = 0.0;
    double fact = 1.0;
    int start_index = 0;
    int end_index = 0;

    // variables for calculation on slave processes' sides
    double local_fact;
    double local_e;
    int received_start = 0;
    int received_end = 0;

    MPI_Status status;
    int root_process = 0;
    int sender = 0;
    int my_id = 0;
    int num_procs = 0;
    int ierr = MPI_Init(&argc, &argv);
    ierr = MPI_Comm_rank(MPI_COMM_WORLD, &my_id);
    ierr = MPI_Comm_size(MPI_COMM_WORLD, &num_procs);

    if (my_id == root_process) {
        double start_time = MPI_Wtime();
        // average number of iterations for every process
        int avg_rows_per_process = num_of_iterations / num_procs + 1;

        // Send ranges for partial sums in slave processes
        for (int an_id = 1; an_id < num_procs; ++an_id) {
            start_index = an_id * avg_rows_per_process + 1;
            end_index = (an_id + 1) * avg_rows_per_process;

            ierr = MPI_Send(&start_index, 1 , MPI_INT,
                             an_id, SEND_TAG, MPI_COMM_WORLD);

            ierr = MPI_Send(&end_index, 1, MPI_INT,
                             an_id, SEND_TAG, MPI_COMM_WORLD);
        }

        for (int i = 1; i <= avg_rows_per_process + 1; ++i) {
            e += 1 / fact;
            fact *= i;
        }

        // Receive partial sums calculated in slave processes
        for (int an_id = 1; an_id < num_procs; ++an_id) {
            ierr = MPI_Recv(&local_e, 1, MPI_DOUBLE, MPI_ANY_SOURCE,
                             MPI_ANY_TAG, MPI_COMM_WORLD, &status);

            sender = status.MPI_SOURCE;

            e += local_e;
        }

        double end_time = MPI_Wtime();

        printf("e = %.20f, elapsed time = %.20fs \n", e, end_time - start_time);
    } else {
        // Receive range
        ierr = MPI_Recv(&received_start, 1, MPI_INT,
                         root_process, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

        ierr = MPI_Recv(&received_end, 1, MPI_INT,
                         root_process, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

        start_index = received_start;
        end_index = received_end > num_of_iterations + 1 ? num_of_iterations : received_end;

        local_e = 0.0;
        local_fact = 2.0;

        for (int i = 3; i < start_index; ++i) {
            local_fact *= i;
        }

        for (int i = start_index; i <= end_index; ++i) {
            local_fact *= i;
            local_e += 1.0 / local_fact;
        }

        ierr = MPI_Send(&local_e, 1, MPI_DOUBLE, root_process,
                         SEND_TAG, MPI_COMM_WORLD);
    }

    ierr = MPI_Finalize();
    return 0;
}
