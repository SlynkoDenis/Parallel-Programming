#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"


#define SEND_TAG 2001

int main(int argc, char** argv) {
    MPI_Status status;
    int root_process = 0;
    int my_id = 0;
    int num_procs = 0;
    int ierr = MPI_Init(&argc, &argv);
    ierr = MPI_Comm_rank(MPI_COMM_WORLD, &my_id);
    ierr = MPI_Comm_size(MPI_COMM_WORLD, &num_procs);

    // TODO: algorithm logic here
    
    ierr = MPI_Finalize();
    return 0;
}
