#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define NUMBER_OF_RADII 8

#define CENT_1 1625
#define CENT_2 1875
#define CENT_5 2125
#define CENT_10 1975
#define CENT_20 2225
#define CENT_50 2425
#define EURO_1 2325
#define EURO_2 2575

struct matrix {
    int rows;
    int cols;
    int faces;
    int *data;
};

void init();

void read_image(FILE *f, struct matrix *output);

void skip_line(FILE *f);

void print_matrix(struct matrix *mat);

void print_matrix_on_file(FILE *f, struct matrix *mat);

void free_matrix(struct matrix *mat);

void increment_accumulator(struct matrix *input, struct matrix *accumulator);

static void fill_sin_cos_arrays();

void write_circles(FILE *f, struct matrix *accumulator, int threshold);
