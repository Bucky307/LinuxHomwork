#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv) {
    int rows, cols;

    FILE *file = fopen(argv[1], "rb");

    if (file == NULL) {
        printf("Error: could not open file.\n");
        exit(1);
    }

    fread(&rows, sizeof(int), 1, file);
    fread(&cols, sizeof(int), 1, file);

    printf("Matrix has %d rows and %d columns.\n", rows, cols);

    double *data = malloc(rows * cols * sizeof(double));
    fread(data, sizeof(double), rows * cols, file);

    printf("Data:\n");
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            printf("%f ", data[i * cols + j]);
        }
        printf("\n");
    }

    fclose(file);
    free(data);

    return 0;
}
