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

void increment_accumulator(struct matrix *input, struct matrix *accumulator) {

    int x, y, a, b;
    float distance_constant = 0.0959F;

    for (int face = 0; face < accumulator->faces; face++) {
        for (int i = 0; i < input->cols * input->rows; i++) {

            if (input->data[i] == 255) {
                x = i % input->cols;
                y = i / input->cols;
                for (int angle = 0; angle < 360; angle++) {
                    a = x - radii_LUT[face] * distance_constant * cos_array[angle];
                    b = y - radii_LUT[face] * distance_constant * sin_array[angle];
                    if ((a > 0) && (a < accumulator->cols - 1) && (b > 0) && (b < accumulator->rows - 1)) {
                        accumulator->data[a + b * accumulator->cols + face * accumulator->cols * accumulator->rows]++;
                        accumulator->data[(a + 1) + b * accumulator->cols + face * accumulator->cols * accumulator->rows]++;
                        accumulator->data[(a - 1) + b * accumulator->cols + face * accumulator->cols * accumulator->rows]++;
                        accumulator->data[a + b * accumulator->cols + face * accumulator->cols * accumulator->rows]++;
                        accumulator->data[a + (b + 1) * accumulator->cols + face * accumulator->cols * accumulator->rows]++;
                        accumulator->data[a + (b - 1) * accumulator->cols + face * accumulator->cols * accumulator->rows]++;
                    }
                }
            }

        }
    }

}

void write_circles(FILE *f, struct matrix *accumulator, int *thresholds) {

    fprintf(f, "x\ty\tradius\n");

    for (int face = 0; face < accumulator->faces; face++) {
        for (int i = 0; i < accumulator->cols * accumulator->rows; i++) {
            // printf("[i(%d), face(%d)] -> acc-data(%d) thresh(%d)\n", i, face, accumulator->data[i + face * accumulator->cols * accumulator->rows], thresholds[face]);
            if (accumulator->data[i + face * accumulator->cols * accumulator->rows] >= thresholds[face] - 15) {
                // printf("YES!\n");
                fprintf(f, "%d\t%d\t%d\n", i % accumulator->cols, i / accumulator->cols, radii_LUT[face]);
            }
        }
    }

}

void find_maximum_by_faces(struct matrix *accumulator, int *result_vector) {

    // result_vector = (int*) malloc(sizeof(int) * accumulator->faces);

    for (int face = 0; face < accumulator->faces; face++) {

        int max = 0;

        for (int i = 0; i < accumulator->rows * accumulator->cols; i++) {

            if (accumulator->data[i + face * accumulator->rows * accumulator->cols] > max) {
                max = accumulator->data[i + face * accumulator->rows * accumulator->cols];
            }
        
        }

        printf("peak for face %d is %d\n", face, max);
        result_vector[face] = max;

    }

}

/*void count_coins(FILE *coords_centers) {

    while(fscanf(coords_centers, "%d%d%d") == 3) {
        
    }

}*/
