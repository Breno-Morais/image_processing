// #include <iostream>
// #include <opencv2/opencv.hpp>
// #include <chrono>
// #include <cmath>

// #include "interface/interface.h"
// #include <gtkmm/application.h>
// #include "image_processing/image_processing.h"

// int main(int argc, char** argv) {
//     auto app = Gtk::Application::create("org.gtkmm.example");

//     return app->make_window_and_run<MainWindow>(argc, argv);
// }

#include <opencv2/opencv.hpp>
using namespace cv;

int main(int argc, char** argv) {
    int camera = -1;
    VideoCapture cap;
    // open the default camera, use something different from 0 otherwise;
    // Check VideoCapture documentation.
    if(!cap.open(camera))
        return 0;

    for(;;) {
        Mat frame;
        cap >> frame;
        if( frame.empty() ) break; // end of video stream
        imshow("This is you, smile! :)", frame);
        if( waitKey(1) == 27 ) break; // stop capturing by pressing ESC
    }

    cap.release(); // release the VideoCapture object
    return 0;
}