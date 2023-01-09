#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

int main(int argc, char **argv)
{
    char *input_filename = "files/placeholder.jpg";
    char *output_filename = "files/edgedetected.jpg";

    if (argc > 1) {
        input_filename = argv[1];
    }

    Mat img = imread(input_filename);
    FileStorage file("files/matrix.txt", cv::FileStorage::WRITE);

    // Convert to graycsale
    Mat img_gray;
    cvtColor(img, img_gray, COLOR_BGR2GRAY);
    // Blur the image for better edge detection
    Mat img_blur;
    GaussianBlur(img_gray, img_blur, Size(7, 7), 0);
    //medianBlur(img_gray, img_blur ,7);
    
    // Canny edge detection
    Mat canny;
    Canny(img_blur, canny, 100, 200, 3, false);

    file << "M" << canny;

    imwrite(output_filename, canny);

    return 0;
}
