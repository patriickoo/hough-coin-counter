#include "hcc_lib.h"

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
    int radius = accumulator->faces / 4;
    int voting_range = 5; // MUST BE ODD NUMBER !
    // printf("radius: %d\n", radius);

    for (int face = 0; face < accumulator->faces; face++) {
        printf("face %d...\n", face);
        for (int i = 0; i < input->cols * input->rows; i++) {

            x = i % input->cols;
            y = i / input->cols;

            if (input->data[i] == 255) {
                
                for (int angle = 0; angle < 360; angle++) {
                    a = x - radius * cos_array[angle];
                    b = y - radius * sin_array[angle];
                    if ((a >= voting_range / 2) && (a < accumulator->cols - (voting_range / 2)) && (b >= voting_range / 2) && (b < accumulator->rows - (voting_range / 2))) {
                        // printf("ab(%d, %d)\n", a, b);

                        for (int i = - (voting_range / 2); i <= voting_range / 2; i++) {
                            for (int j = - (voting_range / 2); j <= voting_range / 2; j++) {
                                // printf("i: %d, j: %d\n", i, j);
                                // printf("[%d, %d, %d] rad(%d), vote(%d), index(%d)\n", x, y, face, radius, voting_range - abs(i) - abs(j), (a + i) + (b + j) * accumulator->cols + face * accumulator->cols * accumulator->rows);
                                accumulator->data[(a + i) + (b + j) * accumulator->cols + face * accumulator->cols * accumulator->rows] += voting_range - abs(i) - abs(j);
                                // printf("val(%d)\n", accumulator->data[(a + i) + (b + j) * accumulator->cols + face * accumulator->cols * accumulator->rows]);
                            }
                        }

                    }
                }
            }

        }
        radius++;
    }

}

int write_circles(struct centers_coords *coords, struct matrix *accumulator, int threshold) {

    int count = 0;
    int radius = accumulator->faces / 10;
    
    for (int face = 0; face < accumulator->faces; face++) {
        for (int i = 0; i < accumulator->cols * accumulator->rows; i++) {
            // printf("[i(%d), face(%d)] -> acc-data(%d) thresh(%d)\n", i, face, accumulator->data[i + face * accumulator->cols * accumulator->rows], thresholds[face]);
            if (accumulator->data[i + face * accumulator->cols * accumulator->rows] >= threshold) {

                coords[count].x = i % accumulator->cols;
                coords[count].y = i / accumulator->cols;
                coords[count].radius = radius;
                printf("%d\t%d\t%d\n", coords[count].x, coords[count].y, coords[count].radius);
                count++;

            }
        }
        radius++;
    }

    realloc(coords, count * sizeof(int) * 3);
    FILE *out_circles;
    out_circles = fopen("files/circle_coordinates.csv", "w");
    write_circles_on_file(out_circles, coords, count);
    return count;

}

void write_circles_on_file(FILE *f, struct centers_coords *coords, int number_of_circles) {

    fprintf(f, "x\ty\tradius\n");

    for (int i = 0; i < number_of_circles; i++) {
        fprintf(f, "%d\t%d\t%d\n", coords[i].x, coords[i].y, coords[i].radius);
    }

}

int find_maximum(struct matrix *accumulator) {

    int max = 0;

    for (int i = 0; i < accumulator->rows * accumulator->cols * accumulator->faces; i++) {
     if (accumulator->data[i] > max) {
            max = accumulator->data[i];
        }
    }

    return max;

}

static int get_distance(struct centers_coords c1, struct centers_coords c2) {

    return (int) sqrt(pow(c1.x - c2.x, 2) + pow(c1.y - c2.y, 2));

}

static float get_value_of_circle(struct centers_coords *coords) {

    float size = (float)coords->radius * DISTANCE_CONSTANT;
    // printf("[%d, %d] size (%d * %f) is %f\n", coords->x, coords->y, coords->radius, DISTANCE_CONSTANT, size);

    if (size >= 812 - RADIUS_TOLERANCE && size <= 812 + RADIUS_TOLERANCE) {
        return 0.01F;
    }

    if (size >= 937 - RADIUS_TOLERANCE && size <= 937 + RADIUS_TOLERANCE) {
        return 0.02F;
    }

    if (size >= 987 - RADIUS_TOLERANCE && size <= 987 + RADIUS_TOLERANCE) {
        return 0.1F;
    }

    if (size >= 1062 - RADIUS_TOLERANCE && size <= 1062 + RADIUS_TOLERANCE) {
        return 0.05F;
    }

    if (size >= 1112 - RADIUS_TOLERANCE && size <= 1112 + RADIUS_TOLERANCE) {
        return 0.2F;
    }

    if (size >= 1162 - RADIUS_TOLERANCE && size <= 1162 + RADIUS_TOLERANCE) {
        return 1.F;
    }

    if (size >= 1212 - RADIUS_TOLERANCE && size <= 1212 + RADIUS_TOLERANCE) {
        return 0.5F;
    }

    if (size >= 1287 - RADIUS_TOLERANCE && size <= 1287 + RADIUS_TOLERANCE) {
        return 2.F;
    }

    return 0;

}

float count_coins(struct centers_coords *coords, int number_of_circles) {

    float total = 0.0F;
    //DEBUG
    int flag;
        
    for (int i = number_of_circles - 1; i >= 0; i--) {

        flag = 0;

        total += get_value_of_circle(&coords[i]);

        for (int j = i + 1; j < number_of_circles; j++) {
            if (get_distance(coords[i], coords[j]) <= coords[i].radius / 2) {
                total -= get_value_of_circle(&coords[i]);
                flag = 1;
                break;
            }
        }

        if (!flag) {
            printf("found %f (%d) in [%d, %d]\n", get_value_of_circle(&coords[i]), coords[i].radius, coords[i].x, coords[i].y);
        }

    }

    return total;

}
