#include "hcc_lib.h"

int main() {

    init();

    // READ IMAGE
    FILE *f;
    f = fopen("files/matrix.txt", "r");
    if (f != NULL) {
        printf("File found: ");
    }
    struct matrix mat;
    read_image(f, &mat);
    fclose(f);
    printf("%dx%d\nIncrementing accumulator...\n", mat.cols, mat.rows);

    // START HOUGH TANSFORM
    struct matrix accumulator;
    accumulator.rows = mat.rows;
    accumulator.cols = mat.cols;
    accumulator.faces = sqrt(pow(mat.rows, 2) + pow(mat.cols, 2)) / 6;
    long long unsigned int data_length = accumulator.rows * accumulator.cols * accumulator.faces;
    printf("data length: %llu\n", data_length);
    accumulator.data = malloc(sizeof *accumulator.data * data_length);
    printf("accumulator: x(%d) y(%d) f(%d) sizeofdata(%lu)\n", accumulator.cols, accumulator.rows, accumulator.faces, sizeof(accumulator.data));
    
    increment_accumulator(&mat, &accumulator);

    printf("Accumulator incremented.\n");

    // FIND MAXIMUM
    int peak = find_maximum(&accumulator);

    printf("Peak is %d\nWriting circles...\n", peak);

    // SAVE CIRCLES COORDINATES AND PRINT ON FILE
    struct centers_coords *coords;
    coords = (struct centers_coords*) malloc(sizeof(int) * 3 * 1000);
    int size = write_circles(coords, &accumulator, (int)((float)peak * 80 / 100));
    printf("Circles written.\n");

    // COUNT COINS
    printf("Subtotal is %f\n", count_coins(coords, size));

    //fclose(print_mat);
    free_matrix(&mat);
    return 0;

}
