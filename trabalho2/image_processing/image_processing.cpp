#include "image_processing.h"

inline uchar ImageProcessor::getLuminance(const cv::Vec3b& pixel) {
    return static_cast<uchar>(0.299*pixel[2] + 0.587*pixel[1] + 0.114*pixel[0]);
}

std::pair<uchar, uchar> ImageProcessor::getMinMaxLuminance(const cv::Mat& src) {
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

void ImageProcessor::mirrorImage(cv::Mat& src, bool horizontalAxis) {
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

void ImageProcessor::invertColor(cv::Vec3b& pixel) {
    pixel[0] = 255 - pixel[0]; // Blue
    pixel[1] = 255 - pixel[1]; // Green
    pixel[2] = 255 - pixel[2]; // Red
}

void ImageProcessor::toGrayScale(cv::Vec3b& pixel) {
    uchar L = getLuminance(pixel);

    pixel[0] = pixel[1] = pixel[2] = L;
}

void ImageProcessor::contrastEnhancement(cv::Vec3b& pixel, float a) {
    for (int i = 0; i < 3; ++i) {
        int newVal = static_cast<int>(a * pixel[i]);
        pixel[i] = cv::saturate_cast<uchar>(newVal);
    }
}

void ImageProcessor::quantizePixel(cv::Vec3b& pixel, int n, uchar minL, float binSize) {
    uchar t_orig = getLuminance(pixel);
    int binIndex = std::min(static_cast<int>((t_orig - (minL - 0.5)) / binSize), n - 1);
    uchar newL = static_cast<uchar>(minL + binIndex * binSize + binSize / 2);

    pixel[0] = pixel[1] = pixel[2] = newL;
}

void ImageProcessor::quantizeImage(cv::Mat& image, int n) {
    auto [min, max] = getMinMaxLuminance(image);
    int tam_int = (max - min + 1); 

    if(n >= tam_int)
        return;

    float bin_size = static_cast<float>(tam_int) / n;

    ImageProcessor::transformImage(image, [n, min, bin_size](cv::Vec3b& pixel) {
        ImageProcessor::quantizePixel(pixel, n, min, bin_size);
    });
} 

void ImageProcessor::quantizeImageColored(cv::Mat& image, int n) {
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

    ImageProcessor::transformImage(image, quantizeColorChannel);
}

cv::Mat ImageProcessor::computeHistogram(const cv::Mat& image, int histSize) {
    cv::Mat hist = cv::Mat::zeros(histSize, 1, CV_32S);
    for(int row = 0; row < image.rows; ++row) {
        for(int col = 0; col < image.cols; ++col) {
            uchar pixelValue = image.at<uchar>(row, col);
            hist.at<int>(pixelValue, 0)++;
        }
    }

    // Normalize now
    float maxVal = 0.0f;
    for (int i = 0; i < histSize; i++) {
        if (hist.at<float>(i, 0) > maxVal) {
            maxVal = hist.at<float>(i, 0);
        }
    }

    int histHeight = 400;
    // Normalize values to [0, histHeight]
    if (maxVal > 0) {
        for (int i = 0; i < histSize; i++) {
            hist.at<float>(i, 0) = (hist.at<float>(i, 0) * histHeight) / maxVal;
        }
    }

    return hist;
}

cv::Mat ImageProcessor::createHistogramImage(const cv::Mat& hist, int histWidth, int histHeight) {
    cv::Mat histImage(histHeight, histWidth, CV_8UC3, cv::Scalar(255, 255, 255));

    int binWidth = cvRound((double)histWidth / hist.rows);

    for (int i = 0; i < hist.rows; i++) {
        int value = cvRound(hist.at<float>(i, 0));
        cv::rectangle(histImage,
            cv::Point(i * binWidth, histHeight),
            cv::Point((i + 1) * binWidth, histHeight - value),
            cv::Scalar(0, 0, 0), 
            cv::FILLED
        );
    }

    return histImage;
}
