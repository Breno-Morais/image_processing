#include "command_interface.h"
#include <iostream>

void CommandInterface::printPrompt() {
    std::cout << "=== Real-Time Video Processor ===\n";
    std::cout << "Commands:\n"
              << " 1 - Gaussian Blur\n"
              << " 2 - Canny Edge\n"
              << " 3 - Sobel Gradient\n"
              << " 4 - Adjust Brightness\n"
              << " 5 - Adjust Contrast\n"
              << " 6 - Negative\n"
              << " 7 - Grayscale\n"
              << " 8 - Resize\n"
              << " 9 - Rotate\n"
              << " h - Mirror Horizontal\n"
              << " v - Mirror Vertical\n"
              << " r - Start/Stop Recording\n"
              << " q - Quit\n";
}

void CommandInterface::handleInput(char key) {
    switch (key) {
        case '1': processor.toggleEffect(VideoProcessor::BLUR); break;
        case '2': processor.toggleEffect(VideoProcessor::CANNY); break;
        case '3': processor.toggleEffect(VideoProcessor::SOBEL); break;
        case '4': processor.toggleEffect(VideoProcessor::BRIGHTNESS); break;
        case '5': processor.toggleEffect(VideoProcessor::CONTRAST); break;
        case '6': processor.toggleEffect(VideoProcessor::NEGATIVE); break;
        case '7': processor.toggleEffect(VideoProcessor::GRAYSCALE); break;
        case '8': processor.toggleEffect(VideoProcessor::RESIZE); break;
        case '9': processor.toggleEffect(VideoProcessor::ROTATE_90); break;
        case 'h': processor.toggleEffect(VideoProcessor::MIRROR_H); break;
        case 'v': processor.toggleEffect(VideoProcessor::MIRROR_V); break;
        case 'r': processor.toggleRecording(); break;
        default: break;
    }
}