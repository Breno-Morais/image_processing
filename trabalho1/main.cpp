#include <iostream>
#include <opencv2/opencv.hpp>

int main(int argc, char** argv) {
    if(argc != 2) {
        std::cerr << "Image path not provided" << std::endl;
        return -1;
    }

    cv::Mat image = cv::imread(argv[1], cv::IMREAD_COLOR);

    if(!image.data) {
        std::cerr << "Could not open or find the image" << std::endl;
        return -1;
    }

    cv::imshow("Display image", image);

    cv::waitKey(0);

    return 0;
}