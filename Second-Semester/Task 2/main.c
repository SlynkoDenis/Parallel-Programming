#include <omp.h>
#include <stdio.h>
#include <stdlib.h>


int main(int argc, char **argv) {
    // 100 is taken as the default value
    int n = 100;
    if (argc == 2) {
        n = atoi(argv[1]);
	    if (n < 1) {
	        fprintf(stderr, "Invalid value from stdin: %d < 1\n", n);
	        n = 100;
	    }
    }

    long sum = 0;
    // variable sum is used in reduction with + operation. It will be copied
    // into threads, initialized there with value 0 and used in threads' cycle parts.
    // In the end thread-local variables will be reducted in the original variable
    #pragma omp parallel for schedule(guided) default(shared) reduction(+: sum)
    for (int i = 1; i <= n; ++i) {
        sum += (long) i;
    }
    printf("Sum = %ld\n", sum);
    return 0;
}
