#include <iostream>
#include <opencv2/opencv.hpp>
#include <chrono>
#include <cmath>

typedef void (*PixelOperation)(cv::Vec3b&);

void mirrorImage(cv::Mat& src, bool horizontalAxis) {
    if(horizontalAxis) {
        for(int y = 0; y < std::floor(src.cols / 2.0); ++y) {
            cv::Mat dummyRow = src.col(y).clone();
            src.col(src.cols - 1 - y).copyTo(src.col(y));
            dummyRow.copyTo(src.col(src.cols - 1 - y));
        }
    } else {
        for(int y = 0; y < std::floor(src.rows / 2.0); ++y) {
            cv::Mat dummyRow = src.row(y).clone();
            src.row(src.rows - 1 - y).copyTo(src.row(y));
            dummyRow.copyTo(src.row(src.rows - 1 - y));
        }
    }
}

void transformImage(cv::Mat& src, PixelOperation processor) {
    for(int y = 0; y < src.rows; ++y) {
        for(int x = 0; x < src.cols; ++x) {
            processor(src.at<cv::Vec3b>(y, x));
        }
    }
}

void invertColor(cv::Vec3b& pixel) {
    pixel[0] = 255 - pixel[0]; // Blue
    pixel[1] = 255 - pixel[1]; // Green
    pixel[2] = 255 - pixel[2]; // Red
}

int main(int argc, char** argv) {
    if(argc < 2) {
        std::cerr << "Image path not provided" << std::endl;
        return -1;
    }

    for(int i = 1; i < argc; ++i) {
        cv::Mat image = cv::imread(argv[i], cv::IMREAD_COLOR);

        if(!image.data) {
            std::cerr << "Could not open or find the first image" << std::endl;
            return -1;
        }

        auto start = std::chrono::high_resolution_clock::now();

        transformImage(image, invertColor);

        auto end = std::chrono::high_resolution_clock::now();

        // Calculate the elapsed time in milliseconds
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        std::cout << "Time taken to mirror the image with entire rows: " << duration.count() << " ms" << std::endl;

        cv::imshow("Image " + std::to_string(i) + " Mirrored", image);
    }
    cv::waitKey(0);


    return 0;
}