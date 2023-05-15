#pragma once

#include <sys/time.h>


// Returns time in range expressed in seconds
double get_elapsed_time(struct timeval begin, struct timeval end);

void print_result(double result, struct timeval begin, struct timeval end,
                  struct timeval join_begin, struct timeval join_end);

// Parses epsilon from input arguments, which must be passed as floating point number in 2nd argument
double get_epsilon(int argc, char** argv);

// Indicates that calculations must be done in a single thread.
// Mode is determined by MODE environment variable equal zero
int is_one_thread_mode();

double pow_int(double base, unsigned int exp);
