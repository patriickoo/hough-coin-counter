#include "hcc_lib.h"

int radii_LUT[] = {
    CENT_1,
    CENT_2,
    CENT_10,
    CENT_5,
    CENT_20,
    EURO_1,
    CENT_50,
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
    }

}

void increment_accumulator(struct matrix *input, struct matrix *accumulator) {

    int x, y, a, b;

    for (int face = 0; face < accumulator->faces; face++) {
        for (int i = 0; i < input->cols * input->rows; i++) {

            if (input->data[i] == 255) {
                x = i % input->cols;
                y = i / input->cols;
                for (int angle = 0; angle < 360; angle++) {
                    a = x - radii_LUT[face] * DISTANCE_CONSTANT * cos_array[angle];
                    b = y - radii_LUT[face] * DISTANCE_CONSTANT * sin_array[angle];
                    if ((a >= 5) && (a < accumulator->cols - 6) && (b >= 5) && (b < accumulator->rows - 6)) {

                        for (int i = -2; i <= 2; i++) {
                            for (int j = -2; j <= 2; j++) {
                                accumulator->data[(a + i) + (b + j) * accumulator->cols + face * accumulator->cols * accumulator->rows]+= 7 - abs(i) - abs(j);
                            }
                        }

                    }
                }
            }

        }
    }

}

int write_circles(struct centers_coords *coords, struct matrix *accumulator, int *thresholds) {

    //fprintf(f, "x\ty\tradius\n");
    int count = 0;
    
    for (int face = 0; face < accumulator->faces; face++) {
        for (int i = 0; i < accumulator->cols * accumulator->rows; i++) {
            // printf("[i(%d), face(%d)] -> acc-data(%d) thresh(%d)\n", i, face, accumulator->data[i + face * accumulator->cols * accumulator->rows], thresholds[face]);
            if (accumulator->data[i + face * accumulator->cols * accumulator->rows] >= (int)((float)(thresholds[face] / 100) * 85)) {

                coords[count].x = i % accumulator->cols;
                coords[count].y = i / accumulator->cols;
                coords[count].radius = radii_LUT[face];
                printf("%d\t%d\t%d\n", coords[count].x, coords[count].y, coords[count].radius);
                count++;

            }
        }
    }

    realloc(coords, count * sizeof(int) * 3);
    FILE *out_circles;
    out_circles = fopen("files/circle_coordinates.csv", "w");
    write_circles_on_file(out_circles, coords, count);
    return count;

}

void write_circles_on_file(FILE *f, struct centers_coords *coords, int size) {

    fprintf(f, "x\ty\tradius\n");

    for (int i = 0; i < size; i++) {
        fprintf(f, "%d\t%d\t%d\n", coords[i].x, coords[i].y, coords[i].radius);
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

static int get_distance(struct centers_coords c1, struct centers_coords c2) {
    
    // printf("\ndist %f\n", sqrt(pow(c1.x - c2.x, 2) + pow(c1.y - c2.y, 2)));
    return (int) sqrt(pow(c1.x - c2.x, 2) + pow(c1.y - c2.y, 2));
    

}

static float get_value_of_circle(struct centers_coords *coords) {

    switch (coords->radius) {
        case 812:
            printf("found 1C [%d, %d]\n", coords->x, coords->y);
            return 0.01F;

        case 937:
            printf("found 2C [%d, %d]\n", coords->x, coords->y);
            return 0.02F;

        case 987:
            printf("found 10C [%d, %d]\n", coords->x, coords->y);
            return 0.1F;

        case 1062:
            printf("found 5C [%d, %d]\n", coords->x, coords->y);
            return 0.05F;

        case 1112:
            printf("found 20C [%d, %d]\n", coords->x, coords->y);
            return 0.2F;

        case 1162:
            printf("found 1E [%d, %d]\n", coords->x, coords->y);
            return 1.F;

        case 1212:
            printf("found 50C [%d, %d]\n", coords->x, coords->y);
            return 0.5F;

        case 1287:
            printf("found 2E [%d, %d]\n", coords->x, coords->y);
            return 2.F;

        default:
            return -9999999999.F;
    }

}

float count_coins(struct centers_coords *coords, int size) {

    float total = 0.0F;

    total += get_value_of_circle(&coords[size - 1]);
        
    for (int i = size - 2; i >= 0; i--) {

        total += get_value_of_circle(&coords[i]);

        for (int j = i + 1; j < size; j++) {
            if (get_distance(coords[i], coords[j]) <= coords[j].radius / 2 * DISTANCE_CONSTANT) {
                printf("\t\t\t\t\t\t\t\t\t\tdelete");
                total -= get_value_of_circle(&coords[i]);
                break;
            }
        }

    }

    return total;

}
