#include "hcc_lib.h"

int main() {

    clock_t start = clock();  // start counting time

    init();  // initialise precomputed sine and cosine arrays

    FILE *f;
    f = fopen("files/matrix.txt", "r");
    if (f != NULL) {
        printf("File found: ");
    }
    struct matrix mat;
    read_image(f, &mat);  // read image and convert it into a struct matrix
    fclose(f);
    printf("%dx%d\nIncrementing accumulator...\n", mat.cols, mat.rows);

    /* HOUGH TANSFORM */
    struct matrix accumulator;
    accumulator.rows = mat.rows;
    accumulator.cols = mat.cols;
    accumulator.faces = sqrt(pow(mat.rows, 2) + pow(mat.cols, 2)) / 6;
    long long unsigned int data_length = accumulator.rows * accumulator.cols * accumulator.faces;
    printf("data length: %llu\n", data_length);
    accumulator.data = malloc(sizeof *accumulator.data * data_length);
    printf("accumulator: x(%d) y(%d) f(%d) sizeofdata(%lu)\n", accumulator.cols, accumulator.rows, accumulator.faces, sizeof(accumulator.data));
    
    increment_accumulator(&mat, &accumulator);  // most of the computing happens here
    printf("Accumulator incremented.\n");
    
    int peak = find_maximum(&accumulator);
    printf("Peak is %d\nWriting circles...\n", peak);

    /* FIND CIRCLES */
    struct centers_coords *coords;
    coords = (struct centers_coords*) malloc(sizeof(int) * 3 * 1000);

    int threshold = (int)((float)peak * 85 / 100); // take the threshold as 85% of the maximum value
    int size = write_circles(coords, &accumulator, threshold);

    printf("Circles written.\n");

    /* COUNT COINS */
    printf("Subtotal is %f\n", count_coins(coords, size));

    clock_t end = clock();
    double time_spent = ((double)(end - start)) / CLOCKS_PER_SEC;

    printf("TOTAL TIME SPENT %lf\n", time_spent);

    free_matrix(&mat);
    free_matrix(&accumulator);

    return 0;

}
