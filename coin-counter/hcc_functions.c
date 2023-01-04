#include "hcc_lib.h"

int radii_LUT[] = {
    CENT_1,
    CENT_2,
    CENT_5,
    CENT_10,
    CENT_20,
    CENT_50,
    EURO_1,
    EURO_2
};

float *sin_array;
float *cos_array;

void init() {
    fill_sin_cos_arrays();
}

void read_image(FILE *f, struct matrix *output) {

    output->faces = 1;

    for (int i = 0; i < 3; i++) {
        skip_line(f);
    }

    fscanf(f,"%*s %d", &output->rows);
    fscanf(f, "%*s %d", &output->cols);
    output->data = (int*) malloc(sizeof(int) * output->rows * output->cols);
    skip_line(f);
    

    for (int i = 0; i < output->rows * output->cols; i++) {
        fscanf(f, "%*s %d", &output->data[i]);
    }

}

void skip_line(FILE *f) {

    fscanf(f, "%*[^\n]\n");

}

void print_matrix(struct matrix *mat) {

    for (int h = 0; h < mat->faces; h++) {
        for (int i = 0; i < mat->rows; i++) {
            for (int j = 0; j < mat->cols; j++) {
                printf("%d\t", mat->data[i * mat->cols + j + h * mat->rows * mat->cols]);
            }
            printf("\n");
        }
        printf("\n");
    }

}

void print_matrix_on_file(FILE *f, struct matrix *mat) {

    for (int h = 0; h < mat->faces; h++) {
        for (int i = 0; i < mat->rows; i++) {
            for (int j = 0; j < mat->cols; j++) {
                fprintf(f, "%d\t", mat->data[i * mat->cols + j + h * mat->rows * mat->cols]);
            }
            fprintf(f, "\n");
        }
        fprintf(f, "\n");
    }

}

void free_matrix(struct matrix *mat) {

    for (int i=0; i < mat->cols * mat->rows * mat->faces; i++) {
        free(&mat->data[i]);
    }
    free(mat);

}

static void fill_sin_cos_arrays() {

    sin_array = (float*) malloc(sizeof(float) * 360);
    cos_array = (float*) malloc(sizeof(float) * 360);

    for (int angle = 0; angle < 360; angle++) {
        sin_array[angle] = (float) sin(angle * M_PI / 180);
        cos_array[angle] = (float) cos(angle * M_PI / 180);
        // printf("%d -> %f\n", angle, sin_array[angle]);
    }

}

/*
void increment_accumulator(struct matrix *input, struct matrix *accumulator) {

    int x, y, a, b;
    float distance_constant = 0.1176699;

    for (int face = 0; face < accumulator->faces; face++){
        for (int i = 0; i < input->cols * input->rows; i++) {

            if (input->data[i] == 255) {
                x = i % input->cols;
                y = i / input->cols;
                //printf("")
                for (int angle = 0; angle < 360; angle++) {
                    a = x - radii_LUT[face] * distance_constant * cos_array[angle];
                    b = y - radii_LUT[face] * distance_constant * sin_array[angle];
                    // printf("[%d, %d] -> [%d, %d]\n", x, y, a, b);
                    if ((a >= 0) && (a <= accumulator->cols) && (b >= 0) && (b <= accumulator->rows)) {
                        accumulator->data[a + b * accumulator->cols + face * accumulator->cols * accumulator->rows]++;
                        // printf("[%d, %d] -> %d\n", a, b, accumulator->data[a + b * accumulator->cols]);
                    }
                }
            }

        }
    }

}
*/

void write_circles(FILE *f, struct matrix *accumulator, int threshold) {

    fprintf(f, "x\ty\tradius\n");

    for (int face = 0; face < accumulator->faces; face++) {
        for (int i = 0; i < accumulator->cols * accumulator->rows; i++) {
            if (accumulator->data[i] >= threshold) {
                fprintf(f, "%d\t%d\t%d\n", i % accumulator->cols, i / accumulator->cols, radii_LUT[face]);
            }
        }
    }

}
