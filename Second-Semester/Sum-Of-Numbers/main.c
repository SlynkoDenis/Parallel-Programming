#include <omp.h>
#include <stdio.h>
#include <stdlib.h>


int main(int argc, char **argv) {
    int n = 100;
    if (argc == 2) {
        n = atoi(argv[1]);
	if (n < 1) {
	    fprintf(stderr, "Value from stdin: %d < 1\n", n);
	    n = 100;
	}
    }
    long sum = 0;
    #pragma omp parallel for \
    schedule(dynamic) \
    default(shared) \
    reduction(+: sum)
    for (int i = 1; i <= n; ++i) {
        sum += (long) i;
    }
    printf("Sum = %ld\n", sum);
    return 0;
}
