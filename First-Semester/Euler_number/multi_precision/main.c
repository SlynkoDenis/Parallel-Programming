#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <gmp.h>
#include "mpi.h"

#define SEND_TAG 2001


int get_number_of_digits(long int number) {
    int counter = 1;
    if (number < 0) {
        ++counter;
    }
    while(number /= 10) {
        ++counter;
    }

    return counter;
}


// Function is necessary for conversion exponent gained from mpf_get_str() function
void dec_to_binary(long int number, char* buff) {
    while (number > 0) {
        *buff = number % 2;
        number /= 2;
        ++buff;
    }
}


// Modify string in such way that mpf_init_set_str() can be executed with this string
char* convert_string_to_mpf(unsigned int len, long int exponent, char* rec_buff) {
    assert(rec_buff);

    int digits_in_exp = get_number_of_digits(exponent);
    char* str_buff = (char *) calloc(len + 4 + sizeof(long int) + digits_in_exp, sizeof(char));
    assert(str_buff);

    str_buff[0] = '0';
    str_buff[1] = '.';
    strcpy(str_buff + 2, rec_buff);
    str_buff[len + 2] = '@';
    // snprintf(str_buff + len + 3, sizeof(long int), "%ld", exponent);
    dec_to_binary(exponent, str_buff + len + 3);

    return str_buff;
}


int main(int argc, char **argv) {
    const int num_of_iterations = 1000;

    // Precision in bits
    const unsigned long base = 8192;
    mpf_set_default_prec(base);

    // variables for calculation on root process side
    mpf_t e, fact, tmp, iter, one;
    mpf_init(e);
    mpf_init(fact);
    mpf_init(tmp);
    mpf_init(iter);
    mpf_init(one);
    mpf_init_set_d(e, 0);
    mpf_init_set_d(fact, 1);
    mpf_init_set_d(tmp, 0);
    mpf_init_set_d(iter, 1);
    mpf_init_set_d(one, 1);

    int start_index = 0;
    int end_index = 0;

    // variables for calculation on slave processes' sides
    mpf_t local_e, local_fact;
    mpf_init(local_e);
    mpf_init(local_fact);
    mpf_init_set_d(local_e, 0);
    mpf_init_set_d(local_fact, 1.0);

    MPI_Status status;
    int root_process = 0;
    int my_id = 0;
    int num_procs = 0;
    int ierr = MPI_Init(&argc, &argv);
    ierr = MPI_Comm_rank(MPI_COMM_WORLD, &my_id);
    ierr = MPI_Comm_size(MPI_COMM_WORLD, &num_procs);

    // average number of iterations for every process
    int avg_rows_per_process = num_of_iterations / num_procs + 1;

    if (my_id == root_process) {
        double start_time = MPI_Wtime();

        for (int i = 1; i <= avg_rows_per_process + 1; ++i) {
            mpf_div(tmp, one, fact);
            mpf_mul(fact, fact, iter);
            mpf_add(iter, iter, one);
            mpf_add(e, e, tmp);
        }

        // Receive partial sums calculated in slave processes
        for (int an_id = 1; an_id < num_procs; ++an_id) {
            // Receive tuple
            int buff_size = base + (1 + 1 + 2) * 4;
            void* packed_buff = calloc(buff_size, 1);
            assert(packed_buff);
            MPI_Recv(packed_buff, buff_size, MPI_PACKED, an_id,
                             MPI_ANY_TAG, MPI_COMM_WORLD, &status);

            // Unpack received data
            int pos = 0;
            unsigned int len = 0;
            long int exponent = 0;
            char* rec_buff = (char *) calloc(base + 1, sizeof(char));
            assert(rec_buff);
            MPI_Unpack(packed_buff, buff_size, &pos, &len, 1, MPI_UNSIGNED, MPI_COMM_WORLD);
            MPI_Unpack(packed_buff, buff_size, &pos, &exponent, 1, MPI_LONG, MPI_COMM_WORLD);
            MPI_Unpack(packed_buff, buff_size, &pos, rec_buff, len + 1, MPI_CHAR, MPI_COMM_WORLD);
            for (int i = len; i < base + 1; ++i) {
                rec_buff[i] = '\0';
            }

            char* str_buff = convert_string_to_mpf(len, exponent, rec_buff);
            mpf_init_set_str(local_e, str_buff, 2);

            mpf_add(e, e, local_e);

            free(rec_buff);
            free(str_buff);
            free(packed_buff);
        }

        double end_time = MPI_Wtime();

        gmp_printf("e = %.Ff, elapsed time = %.20fs\n", e, end_time - start_time);
    } else {
        // Calculate range for execution on current process
        int tmp_end = (my_id + 1) * avg_rows_per_process;
        start_index = my_id * avg_rows_per_process + 1;
        end_index = tmp_end > num_of_iterations + 1 ? num_of_iterations : tmp_end;

        mpf_init_set_d(local_e, 0);
        mpf_init_set_d(local_fact, 2);
        mpf_init_set_d(iter, 3);

        for (int i = 3; i < start_index; ++i) {
            mpf_mul(local_fact, local_fact, iter);
            mpf_add(iter, iter, one);
        }

        for (int i = start_index; i <= end_index; ++i) {
            mpf_mul(local_fact, local_fact, iter);
            mpf_div(tmp, one, local_fact);
            mpf_add(iter, iter, one);
            mpf_add(local_e, local_e, tmp);
        }

        // Convert multiple precision float number to char* for sending via MPI_Send
        mp_exp_t exponent;
        char* buff = mpf_get_str(NULL, &exponent, 2, base, local_e);
        // Due to gmp functions implementation features buff may be with arbitrary length
        unsigned int len = strlen(buff);

        // Pack real length of string, exponent and mantisa into one tuple
        int pos = 0;
        int buff_size = base + (1 + 1 + 2) * 4;
        void* send_buff = calloc(buff_size, 1);
        assert(send_buff);
        MPI_Pack(&len, 1, MPI_UNSIGNED, send_buff, buff_size, &pos, MPI_COMM_WORLD);
        MPI_Pack(&exponent, 1, MPI_LONG, send_buff, buff_size, &pos, MPI_COMM_WORLD);
        MPI_Pack(buff, len + 1, MPI_CHAR, send_buff, buff_size, &pos, MPI_COMM_WORLD);
        
        ierr = MPI_Send(send_buff, buff_size, MPI_PACKED, root_process,
                         SEND_TAG, MPI_COMM_WORLD);
        free(buff);
        free(send_buff);
    }

    mpf_clear(e);
    mpf_clear(fact);
    mpf_clear(local_e);
    mpf_clear(local_fact);
    mpf_clear(one);
    mpf_clear(iter);
    mpf_clear(tmp);

    ierr = MPI_Finalize();
    return 0;
}
