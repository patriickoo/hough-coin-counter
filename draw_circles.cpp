#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

int main(int argc, char **argv)
{
    char *input_filename = "files/placeholder.jpg";

    if (argc > 1) {
        input_filename = argv[1];
    }

    Mat img = imread(input_filename);

    FILE *f;
    f = fopen("files/circle_coordinates.csv", "r");
    int x, y, radius;
    float distance_constant = 0.0959F;

    fscanf(f, "%*s %*s %*s");

    while (fscanf(f, "%d %d %d", &x, &y, &radius) == 3) {

        printf("drawing x(%d) y(%d) rad(%d)\n", x, y, radius);
        Point p = Point(x, y);
        circle(img, p, 1, Scalar(0, 255, 0), 12, LINE_AA);
        circle(img, p, radius * distance_constant, Scalar(255, 0, 0), 2, LINE_AA);

    }

    imshow("circles", img);
    waitKey();

    return 0;
}
