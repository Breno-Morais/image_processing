#include <opencv2/opencv.hpp>
#include <iostream>
#include "core/video_processor.h"
#include "interface/command_interface.h"

using namespace cv;

int main(int argc, char** argv) {
    VideoProcessor processor;

    if(!processor.initializeCamera(-1)) {
        std::cerr << "Error: could not open camera. \n";
        return 1;
    }

    CommandInterface commands(processor);
    namedWindow("Controls", WINDOW_AUTOSIZE);

    int blurVal = 3;
    // O Trackbar precisa ser positivo, então o mapeamento será diferente.

    // O brilho será entre 0 a 200, e será mapeado para -100 a 100
    int brightnessVal = 100; 
    // O contraste irá de 0 a 100, e será mapeado para 0 a 2
    int contrastVal = 50;
    int resizeVal = 100;

    createTrackbar("Blur", "Controls", &blurVal, 25, [](int val, void* userdata) {
        auto* p = static_cast<VideoProcessor*>(userdata);
        p->setBlurSize(val);
    }, &processor);

    createTrackbar("Brightness", "Controls", &brightnessVal, 200, [](int val, void* userdata) {
        auto* p = static_cast<VideoProcessor*>(userdata);
        p->setBrightness(val);
    }, &processor);

    createTrackbar("Contrast", "Controls", &contrastVal, 100, [](int val, void* userdata) {
        auto* p = static_cast<VideoProcessor*>(userdata);
        p->setContrast(val);
    }, &processor);

    createTrackbar("Resize %", "Controls", &resizeVal, 200, [](int val, void* userdata) {
        auto* p = static_cast<VideoProcessor*>(userdata);
        p->setResizePercent(val);
    }, &processor);

    commands.printPrompt();

    while(true) {
        processor.captureFrame();
        processor.applyCurrentEffects();
        processor.displayFrames();

        char key = cv::waitKey(1);
        if (key == 'q' || key == 27) break;  // quit
        commands.handleInput(key);
    }

    processor.release();
    return 0;
}