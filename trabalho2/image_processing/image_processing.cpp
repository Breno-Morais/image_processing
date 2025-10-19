#include "image_processing.h"

// --------------------------------------------------------------------------------------------
// ---------------------------------------- TRABALHO 1 ----------------------------------------
// --------------------------------------------------------------------------------------------
inline uchar ImageProcessor::getLuminance(const cv::Vec3b& pixel) {
    return cv::saturate_cast<uchar>(0.299*pixel[2] + 0.587*pixel[1] + 0.114*pixel[0]);
}

std::pair<uchar, uchar> ImageProcessor::getMinMaxLuminance(const cv::Mat& image) {
    uchar minVal = 255;
    uchar maxVal = 0;

    for(int y = 0; y < image.rows; ++y) {
        for(int x = 0; x < image.cols; ++x) {
            uchar L = getLuminance(image.at<cv::Vec3b>(y, x));
            if(L < minVal) minVal = L;
            if(L > maxVal) maxVal = L;
        }
    }

    return {minVal, maxVal};
}

void ImageProcessor::mirrorImage(cv::Mat& image, bool horizontalAxis) {
    if(horizontalAxis) {
        for(int y = 0; y < std::floor(image.cols / 2.0); ++y) {
            cv::Mat dummyRow = image.col(y).clone();
            image.col(image.cols - 1 - y).copyTo(image.col(y));
            dummyRow.copyTo(image.col(image.cols - 1 - y));
        }
    } else {
        for(int y = 0; y < std::floor(image.rows / 2.0); ++y) {
            cv::Mat dummyRow = image.row(y).clone();
            image.row(image.rows - 1 - y).copyTo(image.row(y));
            dummyRow.copyTo(image.row(image.rows - 1 - y));
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

// --------------------------------------------------------------------------------------------
// ---------------------------------------- TRABALHO 2 ----------------------------------------
// --------------------------------------------------------------------------------------------
// Parte 1
// 1)
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

cv::Mat ImageProcessor::createHistogramImage(const cv::Mat& hist, int histWidth, int histHeight, const cv::Scalar& color, int histSize) {
    cv::Mat histImage(histHeight, histWidth, CV_8UC3, cv::Scalar(255, 255, 255));

    int binWidth = cvRound((double)histWidth / histSize);

    // Rectangle
    for (int i = 0; i < histSize; i++) {
        int value = cvRound(hist.at<float>(i, 0));
        cv::rectangle(
            histImage,
            cv::Point(i * binWidth, histHeight),
            cv::Point((i + 1) * binWidth, histHeight - value),
            cv::Scalar(0, 0, 0),
            cv::FILLED
        );
    }

    // Polyline
    // cv::Point prevPoint(0, histHeight - cvRound(hist.at<float>(0, 0)));
    // for (int i = 1; i < histSize; i++) {
    //     int value = cvRound(hist.at<float>(i, 0));
    //     cv::Point pt(i * binWidth, histHeight - value);
    //     cv::line(histImage, prevPoint, pt, color, 1, cv::LINE_AA);
    //     prevPoint = pt;
    // }

    return histImage;
}

// Just for curiosity
cv::Mat ImageProcessor::createColorHistogramImage(const std::array<cv::Mat,3>& hists, int histWidth, int histHeight, int histSize) {
    cv::Mat histImage(histHeight, histWidth, CV_8UC3, cv::Scalar(255, 255, 255));

    int binWidth = cvRound((double)histWidth / histSize);

    std::array<cv::Scalar,3> colors = {cv::Scalar(255,0,0),  // B
                                       cv::Scalar(0,255,0),  // G
                                       cv::Scalar(0,0,255)}; // R

    for (int c = 0; c < 3; c++) {
        cv::Point prevPoint(0, histHeight - cvRound(hists[c].at<float>(0, 0)));
        for (int i = 1; i < histSize; i++) {
            int value = cvRound(hists[c].at<float>(i, 0));
            cv::Point pt(i * binWidth, histHeight - value);
            cv::line(histImage, prevPoint, pt, colors[c], 1, cv::LINE_AA);
            prevPoint = pt;
        }
    }

    return histImage;
}

// 2)
void ImageProcessor::brightnessEnhancement(cv::Vec3b& pixel, int a) {
    for (int i = 0; i < 3; ++i) {
        int newVal = static_cast<int>(pixel[i] + a);
        pixel[i] = cv::saturate_cast<uchar>(newVal);
    }
}

// 3)
void ImageProcessor::contrastEnhancement(cv::Vec3b& pixel, float a) {
    for (int i = 0; i < 3; ++i) {
        int newVal = static_cast<int>(a * pixel[i]);
        pixel[i] = cv::saturate_cast<uchar>(newVal);
    }
}

// 4)
void ImageProcessor::toNegative(cv::Vec3b& pixel) {
    for (int i = 0; i < 3; ++i) {
        pixel[i] = 255 - pixel[i];
    }
}

// 5)
// Returns the pair: Image Equalized and Histogram Equalized
std::pair<cv::Mat, cv::Mat> ImageProcessor::equalizeHistogram(const cv::Mat& image, int histSize) {
    cv::Mat gray;
    cv::cvtColor(image, gray, cv::COLOR_BGR2GRAY);

    std::vector<int> hist(histSize, 0);
    for (int y = 0; y < gray.rows; y++) {
        for (int x = 0; x < gray.cols; x++) {
            uchar val = gray.at<uchar>(y, x);
            hist[val]++;
        }
    }

    // cumulative histogram
    std::vector<int> cumHist(histSize, 0);
    cumHist[0] = hist[0];
    for (int i = 1; i < histSize; i++)
        cumHist[i] = cumHist[i-1] + hist[i];

    int total = gray.rows * gray.cols;

    // lookup mapping
    std::vector<uchar> lookup(histSize);
    for (int i = 0; i < histSize; i++) {
        lookup[i] = cv::saturate_cast<uchar>(
            ((float)cumHist[i] / total) * (histSize - 1)
        );
    }

    // apply mapping
    cv::Mat eqGray = gray.clone();
    for (int y = 0; y < eqGray.rows; y++) {
        for (int x = 0; x < eqGray.cols; x++) {
            eqGray.at<uchar>(y, x) = lookup[gray.at<uchar>(y, x)];
        }
    }

    // rebuild as 3-channel image
    cv::Mat eqImage;
    cv::cvtColor(eqGray, eqImage, cv::COLOR_GRAY2BGR);

    // build histogram image for visualization
    cv::Mat histMat = ImageProcessor::computeHistogram(eqGray, histSize);
    cv::Mat histImage = ImageProcessor::createHistogramImage(
        histMat, 512, 400, cv::Scalar(0, 0, 0), histSize
    );

    return {eqImage, histImage};
}

std::array<cv::Mat,3> ImageProcessor::computeColorHistograms(const cv::Mat& img, int histSize, int histHeight) {
    CV_Assert(img.type() == CV_8UC3);

    std::array<cv::Mat,3> hists;
    for (int c = 0; c < 3; c++) {
        hists[c] = cv::Mat::zeros(histSize, 1, CV_32F);
        for (int y = 0; y < img.rows; y++) {
            for (int x = 0; x < img.cols; x++) {
                uchar val = img.at<cv::Vec3b>(y, x)[c];
                hists[c].at<float>(val, 0)++;
            }
        }

        // normalize inside compute
        float maxVal = 0;
        for (int i = 0; i < histSize; i++) maxVal = std::max(maxVal, hists[c].at<float>(i, 0));
        for (int i = 0; i < histSize; i++) {
            hists[c].at<float>(i, 0) = (hists[c].at<float>(i, 0) * histHeight) / maxVal;
        }
    }
    return hists;
}

// 6)
ImageProcessor::HistMatchResult ImageProcessor::histogramMatching(const cv::Mat& src, const cv::Mat& ref, int histSize) {
    std::vector<int> histSrc(histSize, 0), histRef(histSize, 0);
    for(int y=0; y<src.rows; y++)
        for(int x=0; x<src.cols; x++)
            histSrc[src.at<uchar>(y,x)]++;
    for(int y=0; y<ref.rows; y++)
        for(int x=0; x<ref.cols; x++)
            histRef[ref.at<uchar>(y,x)]++;

    // CDFs
    std::vector<float> cdfSrc(histSize,0), cdfRef(histSize,0);
    cdfSrc[0] = histSrc[0]; cdfRef[0] = histRef[0];
    for(int i=1;i<histSize;i++){
        cdfSrc[i] = cdfSrc[i-1]+histSrc[i];
        cdfRef[i] = cdfRef[i-1]+histRef[i];
    }
    for(int i=0;i<histSize;i++){
        cdfSrc[i] /= (src.rows*src.cols);
        cdfRef[i] /= (ref.rows*ref.cols);
    }

    // Mapping
    std::vector<uchar> lookup(histSize);
    for(int i=0;i<histSize;i++){
        float val = cdfSrc[i];
        int j=0;
        while(j<histSize-1 && cdfRef[j]<val) j++;
        lookup[i] = j;
    }

    // Apply mapping
    cv::Mat matched = src.clone();
    for(int y=0;y<src.rows;y++)
        for(int x=0;x<src.cols;x++)
            matched.at<uchar>(y,x) = lookup[src.at<uchar>(y,x)];

    // Criar histogramas visuais
    cv::Mat histSrcMat = ImageProcessor::computeHistogram(src, histSize);
    cv::Mat histRefMat = ImageProcessor::computeHistogram(ref, histSize);
    cv::Mat histMatchedMat = ImageProcessor::computeHistogram(matched, histSize);

    cv::Mat histSrcImg = ImageProcessor::createHistogramImage(histSrcMat,    512, 400, cv::Scalar(0,0,0), histSize);
    cv::Mat histRefImg = ImageProcessor::createHistogramImage(histRefMat,    512, 400, cv::Scalar(0,0,0), histSize);
    cv::Mat histMatchedImg = ImageProcessor::createHistogramImage(histMatchedMat,512, 400, cv::Scalar(0,0,0), histSize);

    cv::Mat matched3ch;
    cv::cvtColor(matched, matched3ch, cv::COLOR_GRAY2BGR);

    return {matched3ch, histSrcImg, histRefImg, histMatchedImg};
}
// Parte 2
// 7)
cv::Mat ImageProcessor::zoomOut(const cv::Mat& image, int sx, int sy) {
    int newRows = (image.rows + sy - 1) / sy; // ceil
    int newCols = (image.cols + sx - 1) / sx;

    cv::Mat dst(newRows, newCols, CV_8UC3);

    for (int y = 0; y < newRows; ++y) {
        for (int x = 0; x < newCols; ++x) {
            int startY = y * sy;
            int startX = x * sx;
            int endY = std::min(startY + sy, image.rows);
            int endX = std::min(startX + sx, image.cols);

            long sumB = 0, sumG = 0, sumR = 0;
            int count = 0;

            for (int yy = startY; yy < endY; ++yy) {
                for (int xx = startX; xx < endX; ++xx) {
                    cv::Vec3b pixel = image.at<cv::Vec3b>(yy, xx);
                    sumB += pixel[0];
                    sumG += pixel[1];
                    sumR += pixel[2];
                    count++;
                }
            }

            dst.at<cv::Vec3b>(y, x) = cv::Vec3b(sumB / count, sumG / count, sumR / count);
        }
    }

    return dst;
}

// 8)
cv::Mat ImageProcessor::zoomIn(const cv::Mat& src) {
    int newRows = src.rows * 2 - 1;
    int newCols = src.cols * 2 - 1;

    cv::Mat enlarged(newRows, newCols, CV_8UC3, cv::Scalar(0,0,0));

    // Popula os pixels pares
    for (int y = 0; y < src.rows; ++y) {
        for (int x = 0; x < src.cols; ++x) {
            enlarged.at<cv::Vec3b>(y*2, x*2) = src.at<cv::Vec3b>(y, x);
        }
    }

    // Interpola horizontalmente
    for (int y = 0; y < newRows; y += 2) {
        for (int x = 1; x < newCols; x += 2) {
            cv::Vec3b left = enlarged.at<cv::Vec3b>(y, x-1);
            cv::Vec3b right = enlarged.at<cv::Vec3b>(y, x+1);
            cv::Vec3f avg = (cv::Vec3f)left * 0.5f + (cv::Vec3f)right * 0.5f;
            enlarged.at<cv::Vec3b>(y, x) = cv::Vec3b(
                cv::saturate_cast<uchar>(avg[0]),
                cv::saturate_cast<uchar>(avg[1]),
                cv::saturate_cast<uchar>(avg[2])
            );
        }
    }

    // Interpola verticalmente
    for (int y = 1; y < newRows; y += 2) {
        for (int x = 0; x < newCols; ++x) {
            cv::Vec3b top = enlarged.at<cv::Vec3b>(y-1, x);
            cv::Vec3b bottom = enlarged.at<cv::Vec3b>(y+1, x);
            cv::Vec3f avg = (cv::Vec3f)top * 0.5f + (cv::Vec3f)bottom * 0.5f;
            enlarged.at<cv::Vec3b>(y, x) = cv::Vec3b(
                cv::saturate_cast<uchar>(avg[0]),
                cv::saturate_cast<uchar>(avg[1]),
                cv::saturate_cast<uchar>(avg[2])
            );
        }
    }

    return enlarged;
}

// 9)
cv::Mat ImageProcessor::rotateImage90(const cv::Mat& src, bool clockwise) {
    cv::Mat dst(cv::Size(src.rows, src.cols), src.type());

    for (int y = 0; y < src.rows; ++y) {
        const uchar* srcRow = src.ptr<uchar>(y);
        for (int x = 0; x < src.cols; ++x) {
            int dx, dy;
            if (clockwise) {
                dx = src.rows - 1 - y;
                dy = x;
            } else {
                dx = y;
                dy = src.cols - 1 - x;
            }

            // 3) Copy pixel(s)
            if (src.channels() == 1) {
                dst.at<uchar>(dy, dx) = srcRow[x];
            } else {
                // CV_8UC3: copy B,G,R
                const cv::Vec3b& pix = src.at<cv::Vec3b>(y, x);
                dst.at<cv::Vec3b>(dy, dx) = pix;
            }
        }
    }

    return dst;
}

// 10) A definição dos filtros é feito no arquivo control_panel.cpp na função initialize_presets.
cv::Mat ImageProcessor::convolve(const cv::Mat& src, const cv::Mat& kernel, bool luminanceOnly, bool add127) {
    CV_Assert(kernel.rows == 3 && kernel.cols == 3);

    // saída pode ser grayscale (luminance) ou colorida
    cv::Mat dst = luminanceOnly ? cv::Mat::zeros(src.rows, src.cols, CV_8UC1)
                                : cv::Mat::zeros(src.rows, src.cols, CV_8UC3);

    for (int y = 1; y < src.rows - 1; y++) {
        for (int x = 1; x < src.cols - 1; x++) {
            double acc[3] = {0,0,0};

            for (int ky = -1; ky <= 1; ky++) {
                for (int kx = -1; kx <= 1; kx++) {
                    double w = kernel.at<double>(1 - ky, 1 - kx);

                    if (luminanceOnly) {
                        uchar L = getLuminance(src.at<cv::Vec3b>(y + ky, x + kx));
                        acc[0] += w * L;
                    } else {
                        cv::Vec3b pix = src.at<cv::Vec3b>(y + ky, x + kx);
                        acc[0] += w * pix[0];
                        acc[1] += w * pix[1];
                        acc[2] += w * pix[2];
                    }
                }
            }

            if (luminanceOnly) {
                double val = acc[0];
                if (add127) val += 127;
                val = std::clamp(val, 0.0, 255.0);
                dst.at<uchar>(y, x) = static_cast<uchar>(val);
            } else {
                cv::Vec3b& outPix = dst.at<cv::Vec3b>(y, x);
                for (int c = 0; c < 3; c++) {
                    double val = acc[c];
                    if (add127) val += 127;
                    val = std::clamp(val, 0.0, 255.0);
                    outPix[c] = static_cast<uchar>(val);
                }
            }
        }
    }
    return dst;
}