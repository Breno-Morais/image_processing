#ifndef IMAGE_PROCESSING_H
#define IMAGE_PROCESSING_H

#include <opencv2/opencv.hpp>

class ImageProcessor {
    public:
        static inline uchar getLuminance(const cv::Vec3b& pixel);

        static std::pair<uchar, uchar> getMinMaxLuminance(const cv::Mat& image);

        struct HistMatchResult {
            cv::Mat matchedImage;     // imagem ajustada (3 canais)
            cv::Mat histSrcImage;     // histograma da source
            cv::Mat histRefImage;     // histograma da referência
            cv::Mat histMatchedImage; // histograma da imagem matched
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

        static void brightnessEnhancement(cv::Vec3b& pixel, int a);

        static void toNegative(cv::Vec3b& pixel);

        // Quantize Functions
        static void quantizeImage(cv::Mat& image, int n); 

        static void quantizeImageColored(cv::Mat& image, int n);

        // Histogram
        static cv::Mat computeHistogram(const cv::Mat& image, int histSize = 256);
        static cv::Mat createHistogramImage(const cv::Mat& hist, int histWidth, int histHeight, const cv::Scalar& color, int histSize = 256);
        static cv::Mat createColorHistogramImage(const std::array<cv::Mat,3>& hists, int histWidth, int histHeight, int histSize = 256);
        static cv::Mat equalizeHistogramGray(const cv::Mat& image, int histSize = 256);
        static std::pair<cv::Mat, cv::Mat> equalizeHistogram(const cv::Mat& image, int histSize = 256);
        static std::array<cv::Mat,3> computeColorHistograms(const cv::Mat& img, int histHeight, int histSize = 256);
        static HistMatchResult histogramMatching(const cv::Mat& src, const cv::Mat& ref, int histSize);
        
        // Zoom
        static cv::Mat zoomOut(const cv::Mat& image, int sx, int sy);
        static cv::Mat zoomIn(const cv::Mat& src);

        // Rotation
        static cv::Mat rotateImage90(const cv::Mat& src, bool clockwise);

        // Convolution
        static cv::Mat convolve(const cv::Mat& src, const cv::Mat& kernel, bool luminanceOnly, bool add127);
};

#endif // IMAGE_PROCESSING_H