#include "video_processor.h"

#include "../image_processing/image_processing.h"

using namespace cv;

bool VideoProcessor::initializeCamera(int cameraIndex) {
    if (!cap.open(cameraIndex)) {
        std::cerr << "Error: Could not open camera.\n";
        return false;
    }

    // Create the display windows once
    namedWindow("Original", WINDOW_AUTOSIZE);
    namedWindow("Processed", WINDOW_AUTOSIZE);

    // Initialize all effects as OFF
    for (bool &flag : effects)
        flag = false;

    return true;
}

void VideoProcessor::captureFrame() {
    cap >> frame;
    if (frame.empty()) return;

    frame.copyTo(processed);
}

void VideoProcessor::applyCurrentEffects() {
    if (frame.empty()) return;

    // --- Gaussian Blur ---
    if (effects[BLUR]) {
        int k = blurSize % 2 == 0 ? blurSize + 1 : blurSize;
        GaussianBlur(processed, processed, Size(k, k), 0);
    }

    // --- Sobel Gradient ---
    if (effects[SOBEL]) {
        Mat gray;

        // A imagem pode já ter sido alterada
        if (processed.channels() == 3)
            cvtColor(processed, gray, COLOR_BGR2GRAY);
        else
            gray = processed;

        Mat gradX, gradY, grad;
        Sobel(gray, gradX, CV_16S, 1, 0);
        Sobel(gray, gradY, CV_16S, 0, 1);
        convertScaleAbs(gradX, gradX);
        convertScaleAbs(gradY, gradY);
        addWeighted(gradX, 0.5, gradY, 0.5, 0, grad);

        cvtColor(grad, processed, COLOR_GRAY2BGR);  // Always safe
    }

    // --- Brightness / Contrast ---
    if (effects[BRIGHTNESS]) {
        processed.convertTo(processed, -1, 1, beta);
    }

    if (effects[CONTRAST]) {
        processed.convertTo(processed, -1, alpha, 0);
    }

    // --- Negative ---
    if (effects[NEGATIVE]) {
        processed = Scalar::all(255) - processed;
    }

    // --- Grayscale ---
    if (effects[GRAYSCALE]) {
        cvtColor(processed, processed, COLOR_BGR2GRAY);
        cvtColor(processed, processed, COLOR_GRAY2BGR);
    }

    // --- Canny Edge Detection ---
    if (effects[CANNY]) {
        Mat edges;
        Canny(processed, edges, 50, 150);
        cvtColor(edges, processed, COLOR_GRAY2BGR);
    }


    // --- Resize ---
    if (effects[RESIZE]) {
        double scale = resizePercent / 100.0;
        resize(processed, processed, Size(), scale, scale, INTER_LINEAR);
    }

    // --- Rotate (90 degrees) ---
    if (effects[ROTATE_90]) {
        rotate(processed, processed, ROTATE_90_CLOCKWISE);
    }

    // --- Mirror (horizontal + vertical) ---
    if (effects[MIRROR]) {
        flip(processed, processed, -1);
    }

    // --- Recording ---
    if (recording && writer.isOpened()) {
        writer.write(processed);
    }
}

void VideoProcessor::displayFrames() {
    if (frame.empty()) return;

    imshow("Original", frame);
    imshow("Processed", processed);
}

void VideoProcessor::toggleEffect(EffectType effect) {
    if (effect >= 0 && effect < NUM_EFFECTS) {
        effects[effect] = !effects[effect];
        std::cout << "[INFO] " << getEffectName(effect)
             << " set to " << (effects[effect] ? "ON" : "OFF") << '\n';
    }
}

void VideoProcessor::adjustParameter(std::string name, int value) {
    if (name == "blur") {
        blurSize = value;
        std::cout << "Blur kernel: " << blurSize << '\n';
    } else if (name == "brightness") {
        beta = value - 100; // shift brightness
        std::cout << "Brightness: " << beta << '\n';
    } else if (name == "contrast") {
        alpha = value / 50.0; // scale contrast
        std::cout << "Contrast: " << alpha << '\n';
    }
}

void VideoProcessor::toggleRecording() {
    recording = !recording;

    if (recording) {
        int frame_width = processed.cols;
        int frame_height = processed.rows;
        writer.open("output.avi", VideoWriter::fourcc('M','J','P','G'), 30, Size(frame_width, frame_height));
        if (!writer.isOpened()) {
            std::cerr << "Could not open output file for writing.\n";
            recording = false;
        } else {
            std::cout << "Recording started.\n";
        }
    } else {
        writer.release();
        std::cout << "Recording stopped.\n";
    }
}

void VideoProcessor::release() {
    cap.release();
    writer.release();
    destroyAllWindows();
}

std::string VideoProcessor::getEffectName(EffectType effect) const {
    switch (effect) {
        case BLUR: return "Gaussian Blur";
        case CANNY: return "Canny Edge";
        case SOBEL: return "Sobel Gradient";
        case BRIGHTNESS: return "Brightness";
        case CONTRAST: return "Contrast";
        case NEGATIVE: return "Negative";
        case GRAYSCALE: return "Grayscale";
        case RESIZE: return "Resize (1/2)";
        case ROTATE_90: return "Rotate 90°";
        case MIRROR: return "Mirror";
        default: return "Unknown";
    }
}

void VideoProcessor::setBlurSize(int k) {
    if (k <= 0) k = 1;
    blurSize = (k % 2 == 0) ? k + 1 : k;
}

void VideoProcessor::setBrightness(int b) {
    beta = b - 100; // map [0,200] para [-100,100]
}

void VideoProcessor::setContrast(double a) {
    alpha = a / 50.0; // map [0,100] para [0.0, 2.0]
}

void VideoProcessor::setResizePercent(int p) {
    resizePercent = std::max(10, p); // avoid 0
}