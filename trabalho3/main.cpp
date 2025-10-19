#include <opencv2/opencv.hpp>
#include <iostream>
#include "core/video_processor.h"
#include "interface/command_interface.h"
#include "interface/interface.h"

using namespace cv;

int main(int argc, char** argv) {
    VideoProcessor processor;

    if(!processor.initializeCamera(-1)) {
        std::cerr << "Error: could not open camera. \n";
        return 1;
    }

    CommandInterface commands(processor);
    GtkInterface ui(processor);
    ui.run();

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