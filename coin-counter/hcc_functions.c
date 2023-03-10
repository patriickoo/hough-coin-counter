#include "hcc_lib.h"

float *sin_array;
float *cos_array;

void init() {
    fill_sin_cos_arrays();  // precompute sine and cosine values. Much more efficient
}

/* converts an image as a file into an image as a matrix (flattened into an array) */
void read_image(FILE *f, struct matrix *output) {

    output->faces = 1;

    for (int i = 0; i < 3; i++) {
        skip_line(f);  // the file generated by opencv has some starting lines which we must skip
    }

    fscanf(f,"%*s %d", &output->rows);  // we read from the file how many columns
    fscanf(f, "%*s %d", &output->cols);  // and rows we have
    output->data = (int*) malloc(sizeof(int) * output->rows * output->cols);  // then allocate the correct amount of memory
    skip_line(f);  // we must skip one more line before starting
    

    for (int i = 0; i < output->rows * output->cols; i++) {
        fscanf(f, "%*s %d", &output->data[i]);  // now for each pixel we read its value and store it in the array
    }

}

static inline void skip_line(FILE *f) {

    fscanf(f, "%*[^\n]\n");

}

/* prints a matrix in the console */
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

/* prints a matrix on a given file */
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

/* deallocates space assigned to given matrix to avoid memory leaks */
void free_matrix(struct matrix *mat) {

    for (int i=0; i < mat->cols * mat->rows * mat->faces; i++) {
        free(&mat->data[i]);
    }
    free(&mat->data);

}

/* fills in sin and cos arrays */
static void fill_sin_cos_arrays() {

    sin_array = (float*) malloc(sizeof(float) * 360);  // allocating one float for each angle
    cos_array = (float*) malloc(sizeof(float) * 360);

    for (int angle = 0; angle < 360; angle++) {
        sin_array[angle] = (float) sin(angle * M_PI / 180);  // savings its value in the array
        cos_array[angle] = (float) cos(angle * M_PI / 180);
    }

}

/* increments the accumulator matrix using edge detected image as input */
void increment_accumulator(struct matrix *input, struct matrix *accumulator) {

    int x, y, a, b;
    int radius = accumulator->faces / MIN_RADIUS_CONSTANT;  // the starting radius is relative to the size of the image
    int voting_range = 5; // MUST BE ODD NUMBER !

    for (int face = 0; face < accumulator->faces; face++) {
        printf("face %d...\n", face);
        for (int i = 0; i < input->cols * input->rows; i++) {

            x = i % input->cols;
            y = i / input->cols;

            if (input->data[i] == 255) {  // if current cell is an edge, increment accumulator in a circle pattern around it
                
                for (int angle = 0; angle < 360; angle++) {
                    a = x - radius * cos_array[angle];  // derived from circle equation
                    b = y - radius * sin_array[angle];
                    if (
                        (a >= voting_range / 2) && (a < accumulator->cols - (voting_range / 2)) 
                        && 
                        (b >= voting_range / 2) && (b < accumulator->rows - (voting_range / 2))
                        ) {  // only keep a and b couples which are both inside correct bounds

                        // incrementing accumulator in a weighted square pattern around each [a, b] couple
                        for (int i = - (voting_range / 2); i <= voting_range / 2; i++) {
                            for (int j = - (voting_range / 2); j <= voting_range / 2; j++) {
                                accumulator->data[
                                    (a + i) + (b + j) * accumulator->cols + face * accumulator->cols * accumulator->rows
                                    ] += voting_range - abs(i) - abs(j);
                            }
                        }

                    }
                }
            }

        }
        radius++;  // radius is incremented each time the face changes
    }

}

/* fills in coordinates struct and returns number of circles, starting from accumulator matrix and threshold */
int write_circles(struct centers_coords *coords, struct matrix *accumulator, int threshold) {

    int count = 0;
    int radius = accumulator->faces / MIN_RADIUS_CONSTANT;
    
    for (int face = 0; face < accumulator->faces; face++) {
        for (int i = 0; i < accumulator->cols * accumulator->rows; i++) {
            if (accumulator->data[i + face * accumulator->cols * accumulator->rows] >= threshold) {
                // if the current accumulator cell is greater than the threshold
                coords[count].x = i % accumulator->cols;
                coords[count].y = i / accumulator->cols;
                coords[count].radius = radius;  // add circle coordinates to struct
                printf("%d\t%d\t%d\n", coords[count].x, coords[count].y, coords[count].radius);
                count++;  // and increment struct index

            }
        }
        radius++;  // radius is incremented every time the face changes
    }

    realloc(coords, count * sizeof(int) * 3);
    FILE *out_circles;
    out_circles = fopen("files/circle_coordinates.csv", "w");
    write_circles_on_file(out_circles, coords, count);  // generate .csv file
    return count;  // return number of circles found

}

/* creates a tab-separated .csv file with given circles */
void write_circles_on_file(FILE *f, struct centers_coords *coords, int number_of_circles) {

    fprintf(f, "x\ty\tradius\n");

    for (int i = 0; i < number_of_circles; i++) {
        fprintf(f, "%d\t%d\t%d\n", coords[i].x, coords[i].y, coords[i].radius);
    }

}

/* returns the maximum value contained inside given matrix */
int find_maximum(struct matrix *mat) {

    int max = 0;

    for (int i = 0; i < mat->rows * mat->cols * mat->faces; i++) {
        if (mat->data[i] > max) {
            max = mat->data[i];
        }
    }

    return max;

}

/* returns the distance between two circles */
static inline int get_distance(struct centers_coords c1, struct centers_coords c2) {

    return (int) sqrt(pow(c1.x - c2.x, 2) + pow(c1.y - c2.y, 2));

}

/* returns the value, if any, of the given circle */
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

/* given the found circles, returns the sum of their values in euro */
float count_coins(struct centers_coords *coords, int number_of_circles) {

    float total = 0.0F;
    int flag;  // debug
        
    for (int i = number_of_circles - 1; i >= 0; i--) {  // going backwards, so bigger circles come first

        flag = 0;  // debug

        float value;

        value = get_value_of_circle(&coords[i]);

        if (value == 0) {  // if value is 0, remove circle so we don't exclude coins improperly
            coords[i].radius = 0;
            coords[i].x = 0;
            coords[i].y = 0;
            flag = 1;
        }

        total += value;  //then sum it to the total

        for (int j = i + 1; j < number_of_circles; j++) {  // cycle through previous bigger coins
            if (get_distance(coords[i], coords[j]) <= coords[i].radius / 2) {  // if one is found close to the current one
                total -= get_value_of_circle(&coords[i]);  // remove it from the total (it was wrongly added)
                flag = 1;  // debug
                break;
            }
        }

        /* DEBUG */
        if (!flag) {
            printf("found %f (%d) in [%d, %d]\n", get_value_of_circle(&coords[i]), coords[i].radius, coords[i].x, coords[i].y);
        }

    }

    return total;

}
