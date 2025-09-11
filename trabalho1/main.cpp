#include <iostream>
#include <opencv2/opencv.hpp>
#include <chrono>
#include <cmath>

inline uchar getLuminance(const cv::Vec3b& pixel) {
    return static_cast<uchar>(0.299*pixel[2] + 0.587*pixel[1] + 0.114*pixel[0]);
}

std::pair<uchar, uchar> getMinMaxLuminance(const cv::Mat& src) {
    uchar minVal = 255;
    uchar maxVal = 0;

    for(int y = 0; y < src.rows; ++y) {
        for(int x = 0; x < src.cols; ++x) {
            uchar L = getLuminance(src.at<cv::Vec3b>(y, x));
            if(L < minVal) minVal = L;
            if(L > maxVal) maxVal = L;
        }
    }

    return {minVal, maxVal};
}

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

template <typename PixelOperation>
void transformImage(cv::Mat& src, PixelOperation processor) {
    for (int y = 0; y < src.rows; ++y) {
        for (int x = 0; x < src.cols; ++x) {
            processor(src.at<cv::Vec3b>(y, x));
        }
    }
}

void invertColor(cv::Vec3b& pixel) {
    pixel[0] = 255 - pixel[0]; // Blue
    pixel[1] = 255 - pixel[1]; // Green
    pixel[2] = 255 - pixel[2]; // Red
}

void toGrayScale(cv::Vec3b& pixel) {
    uchar L = getLuminance(pixel);

    pixel[0] = pixel[1] = pixel[2] = L; // Red
}

void contrastEnhancement(cv::Vec3b& pixel, float a) {
    for (int i = 0; i < 3; ++i) {
        int newVal = static_cast<int>(a * pixel[i]);
        pixel[i] = cv::saturate_cast<uchar>(newVal);
    }
}

void quantizeColor(cv::Vec3b& pixel, int n, uchar minL, float binSize) {
    uchar t_orig = getLuminance(pixel);
    int binIndex = std::min(static_cast<int>((t_orig - (minL - 0.5)) / binSize), n - 1);
    uchar newL = static_cast<uchar>(minL + binIndex * binSize + binSize / 2);

    pixel[0] = pixel[1] = pixel[2] = newL;
}

void quantizeImage(cv::Mat& image, int n) {
    auto [min, max] = getMinMaxLuminance(image);
    int tam_int = (max - min + 1); 

    if(n >= tam_int)
        return;

    float bin_size = static_cast<float>(tam_int) / n;

    transformImage(image, [n, min, bin_size](cv::Vec3b& pixel) {
        quantizeColor(pixel, n, min, bin_size);
    });
} 

void quantizeImageColored(cv::Mat& image, int n) {
    if (n <= 1)
        return;

    auto quantizeColorChannel = [n](cv::Vec3b& pixel) {
        for (int i = 0; i < 3; ++i) {
            uchar minVal = 0;
            uchar maxVal = 255;
            float binSize = static_cast<float>(maxVal - minVal + 1) / n;

            int binIndex = std::min(static_cast<int>((pixel[i] - (minVal - 0.5)) / binSize), n - 1);
            pixel[i] = static_cast<uchar>(minVal + binIndex * binSize + binSize / 2);
        }
    };

    transformImage(image, quantizeColorChannel);
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

        cv::imshow("Image " + std::to_string(i), image);

        auto start = std::chrono::high_resolution_clock::now();

        quantizeImage(image, 4);

        auto end = std::chrono::high_resolution_clock::now();

        // Calculate the elapsed time in milliseconds
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        std::cout << "Time taken: " << duration.count() << " ms" << std::endl;

        cv::imshow("Image " + std::to_string(i) + " Quantized", image);
    }
    cv::waitKey(0);


    return 0;
}