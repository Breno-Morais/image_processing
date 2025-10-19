#ifndef VIDEO_PROC
#define VIDEO_PROC

#include <opencv2/opencv.hpp>


class VideoProcessor {
    public:
        enum EffectType {
            BLUR,
            CANNY,
            SOBEL,
            BRIGHTNESS,
            CONTRAST,
            NEGATIVE,
            GRAYSCALE,
            RESIZE_HALF,
            ROTATE_90,
            MIRROR,
            NUM_EFFECTS
        };

        bool initializeCamera(int cameraIndex);
        void captureFrame();
        void applyCurrentEffects();
        void displayFrames();
        void toggleEffect(EffectType effect);
        void toggleRecording();
        void adjustParameter(std::string name, int value);
        void release();

    private:
        std::string getEffectName(EffectType effect) const;

        cv::VideoCapture cap;
        cv::VideoWriter writer;
        cv::Mat frame, processed;
        bool effects[NUM_EFFECTS] = {false};
        int blurSize = 3;
        double alpha = 1.0, beta = 0.0; // contrast and brightness
        bool recording = false;
};

#endif /* VIDEO_PROC */