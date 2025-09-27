#ifndef IMAGE_PROCESSING_H
#define IMAGE_PROCESSING_H

#include <opencv2/opencv.hpp>

class ImageProcessor {
    public:
        static inline uchar getLuminance(const cv::Vec3b& pixel);

        static std::pair<uchar, uchar> getMinMaxLuminance(const cv::Mat& src);

        enum class ImageOperation {
            MIRROR = 1,
            TO_GRAYSCALE = 2,
            CONTRAST_ENHANCEMENT = 3,
            QUANTIZE_GRAYSCALE = 4,
            QUANTIZE_COLOR = 5
        };

        static void mirrorImage(cv::Mat& src, bool horizontalAxis);

        template <typename PixelOperation>
        static void transformImage(cv::Mat& src, PixelOperation processor) {
            for (int y = 0; y < src.rows; ++y) {
                for (int x = 0; x < src.cols; ++x) {
                    processor(src.at<cv::Vec3b>(y, x));
                }
            }
        }
        
        // Pixel Transformations
        static void invertColor(cv::Vec3b& pixel);

        static void toGrayScale(cv::Vec3b& pixel);

        static void contrastEnhancement(cv::Vec3b& pixel, float a);

        static void quantizePixel(cv::Vec3b& pixel, int n, uchar minL, float binSize);

        // Quantize Functions
        static void quantizeImage(cv::Mat& image, int n); 

        static void quantizeImageColored(cv::Mat& image, int n);
};

#endif // IMAGE_PROCESSING_H