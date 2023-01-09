#include "hcc_lib.h"

int main() {

    init();

    // READ IMAGE
    FILE *f;
    f = fopen("files/matrix.txt", "r");
    if (f != NULL) {
        printf("ok file\n");
    }
    struct matrix mat;
    read_image(f, &mat);
    fclose(f);
    printf("number of rows is %d\n", mat.rows);
    printf("number of cols is %d\n", mat.cols);

    // PRINT MATRIX ON FILE
    FILE *print_mat;
    print_mat = fopen("files/output_matrix.csv", "w");
    print_matrix_on_file(print_mat, &mat);

    // START HOUGH TANSFORM
    struct matrix accumulator;
    accumulator.rows = mat.rows;
    accumulator.cols = mat.cols;
    accumulator.faces = NUMBER_OF_RADII;
    accumulator.data = (int*) malloc(accumulator.rows * accumulator.cols * accumulator.faces * sizeof(int));
    
    increment_accumulator(&mat, &accumulator);

    // PRINT ACCUMULATOR ON FILE
    FILE *print_acc;
    print_acc = fopen("files/accumulator_matrix.csv", "w");
    print_matrix_on_file(print_acc, &accumulator);

    // FIND MAXIMUM PEAK
    int *peaks;
    peaks = (int*) malloc(sizeof(int) * accumulator.faces);
    find_maximum_by_faces(&accumulator, peaks);

    // SAVE CIRCLES COORDINATES AND PRINT ON FILE
    struct centers_coords *coords;
    coords = (struct centers_coords*) malloc(sizeof(int) * 3 * 1000);
    int size = write_circles(coords, &accumulator, peaks);

    // COUNT COINS
    printf("the subtotal is %f\n", count_coins(coords, size));

    //fclose(print_mat);
    free_matrix(&mat);
    return 0;

}
