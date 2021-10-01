#include <stdlib.h>
#include <stdio.h>
#include <time.h>


int main() {
    const char *filename = "sample.dat";
    const int n = 10000;

    FILE *file = fopen(filename, "w");
    if (file) {
        srand((unsigned int)time(NULL) / 2);
        fprintf(file, "%d ", n);
        for (int i = 0; i < n; ++i) {
            fprintf(file, "%d ", rand());
        }
        fclose(file);
    } else {
        fprintf(stderr, "Failed to open file %s\n", filename);
        return 1;
    }

    return 0;
}
