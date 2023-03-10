#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

#define RESIZE_CONSTANT 0.8F
#define DISTANCE_CONSTANT 6.4375F / RESIZE_CONSTANT
#define RADIUS_TOLERANCE 10
#define MIN_RADIUS_CONSTANT 4

// each of these is the radius of the coin in millimetres
#define CENT_1 1625 / 2
#define CENT_2 1875 / 2
#define CENT_5 2125 / 2
#define CENT_10 1975 / 2
#define CENT_20 2225 / 2
#define CENT_50 2425 / 2
#define EURO_1 2325 / 2
#define EURO_2 2575 / 2

struct matrix {
    int rows;
    int cols;
    int faces;
    int *data;
};

struct centers_coords {
    int x;
    int y;
    int radius;
};

void init();

void read_image(FILE *f, struct matrix *output);

static inline void skip_line(FILE *f);

void print_matrix(struct matrix *mat);

void print_matrix_on_file(FILE *f, struct matrix *mat);

void free_matrix(struct matrix *mat);

void increment_accumulator(struct matrix *input, struct matrix *accumulator);

static void fill_sin_cos_arrays();

int write_circles(struct centers_coords *coords, struct matrix *accumulator, int threshold);

int find_maximum(struct matrix *accumulator);

static inline int get_distance(struct centers_coords c1, struct centers_coords c2);

float count_coins(struct centers_coords *coords, int size);

void write_circles_on_file(FILE *f, struct centers_coords *coords, int size);
