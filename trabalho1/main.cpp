#include <iostream>
#include <opencv2/opencv.hpp>
#include <chrono>
#include <cmath>

// ==========================
//  === Utility Functions ===
// ==========================
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

enum class ImageOperation {
    MIRROR = 1,
    TO_GRAYSCALE = 2,
    CONTRAST_ENHANCEMENT = 3,
    QUANTIZE_GRAYSCALE = 4,
    QUANTIZE_COLOR = 5
};

// =====================================
//  == Image Transformation Functions ==
// =====================================
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

// ==========================
// ==== Pixel Operations ====
// ==========================
void invertColor(cv::Vec3b& pixel) {
    pixel[0] = 255 - pixel[0]; // Blue
    pixel[1] = 255 - pixel[1]; // Green
    pixel[2] = 255 - pixel[2]; // Red
}

void toGrayScale(cv::Vec3b& pixel) {
    uchar L = getLuminance(pixel);

    pixel[0] = pixel[1] = pixel[2] = L;
}

void contrastEnhancement(cv::Vec3b& pixel, float a) {
    for (int i = 0; i < 3; ++i) {
        int newVal = static_cast<int>(a * pixel[i]);
        pixel[i] = cv::saturate_cast<uchar>(newVal);
    }
}

void quantizePixel(cv::Vec3b& pixel, int n, uchar minL, float binSize) {
    uchar t_orig = getLuminance(pixel);
    int binIndex = std::min(static_cast<int>((t_orig - (minL - 0.5)) / binSize), n - 1);
    uchar newL = static_cast<uchar>(minL + binIndex * binSize + binSize / 2);

    pixel[0] = pixel[1] = pixel[2] = newL;
}

// ==============================
// === Quantization Functions ===
// ==============================
void quantizeImage(cv::Mat& image, int n) {
    auto [min, max] = getMinMaxLuminance(image);
    int tam_int = (max - min + 1); 

    if(n >= tam_int)
        return;

    float bin_size = static_cast<float>(tam_int) / n;

    transformImage(image, [n, min, bin_size](cv::Vec3b& pixel) {
        quantizePixel(pixel, n, min, bin_size);
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

// ======================
// === Main Function ===
// ======================
int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <image_path1> <image_path2> ..." << std::endl;
        return -1;
    }

    std::cout << "Choose an operation to perform on the images:\n";
    std::cout << static_cast<int>(ImageOperation::MIRROR) << ". Mirror the image\n";
    std::cout << static_cast<int>(ImageOperation::TO_GRAYSCALE) << ". Convert to Grayscale\n";
    std::cout << static_cast<int>(ImageOperation::CONTRAST_ENHANCEMENT) << ". Enhance Contrast\n";
    std::cout << static_cast<int>(ImageOperation::QUANTIZE_GRAYSCALE) << ". Quantize (Grayscale)\n";
    std::cout << static_cast<int>(ImageOperation::QUANTIZE_COLOR) << ". Quantize (Color)\n";
    std::cout << "Enter your choice: ";

    int choice;
    std::cin >> choice;

    for (int i = 1; i < argc; ++i) {
        cv::Mat image = cv::imread(argv[i], cv::IMREAD_COLOR);
        cv::Mat originalImage = image.clone();

        if (!image.data) {
            std::cerr << "Could not open or find the image: " << argv[i] << std::endl;
            continue;
        }

        switch (static_cast<ImageOperation>(choice)) {
            case ImageOperation::MIRROR: {
                std::cout << "Mirror horizontally (1) or vertically (0): ";
                bool horizontal;
                std::cin >> horizontal;
                mirrorImage(image, horizontal);
                break;
            }
            case ImageOperation::TO_GRAYSCALE:
                transformImage(image, toGrayScale);
                break;
            case ImageOperation::CONTRAST_ENHANCEMENT: {
                float contrastFactor;
                std::cout << "Enter contrast factor: ";
                std::cin >> contrastFactor;
                transformImage(image, [contrastFactor](cv::Vec3b& pixel) {
                    contrastEnhancement(pixel, contrastFactor);
                });
                break;
            }
            case ImageOperation::QUANTIZE_GRAYSCALE: {
                int n;
                std::cout << "Enter number of grayscale levels: ";
                std::cin >> n;
                quantizeImage(image, n);
                break;
            }
            case ImageOperation::QUANTIZE_COLOR: {
                int n;
                std::cout << "Enter number of color levels: ";
                std::cin >> n;
                quantizeImageColored(image, n);
                break;
            }
            default:
                std::cerr << "Invalid choice." << std::endl;
                continue;
        }

        std::cout << "Do you want to save the processed image? (y/n): ";
        char saveChoice;
        std::cin >> saveChoice;

        if (saveChoice == 'y' || saveChoice == 'Y') {
            std::string outputPath;
            std::cout << "Enter the output file path (it will be saved as a jpg): ";
            std::cin >> outputPath;

            outputPath += ".jpg";

            if (cv::imwrite(outputPath, image)) {
                std::cout << "Image saved successfully to " << outputPath << std::endl;
            } else {
                std::cerr << "Failed to save the image. Please check the file path and permissions." << std::endl;
            }
        }

        int padding = 10; // Space between the images
        cv::copyMakeBorder(originalImage, originalImage, 0, 0, 0, padding, cv::BORDER_CONSTANT, cv::Scalar(0, 0, 0));

        cv::Mat combinedImage;
        cv::hconcat(originalImage, image, combinedImage);

        std::string windowName = "Original and Processed Image - " + std::to_string(i);
        cv::namedWindow(windowName, cv::WINDOW_NORMAL);
        cv::resizeWindow(windowName, combinedImage.cols, combinedImage.rows);
        cv::imshow(windowName, combinedImage);
    }

    cv::waitKey(0);
    return 0;
}