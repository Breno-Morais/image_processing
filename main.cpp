#include <iostream>
#include <opencv2/opencv.hpp>
#include <chrono>
#include <cmath>

#include "interface/interface.h"
#include <gtkmm/application.h>
#include "image_processing/image_processing.h"

int main(int argc, char** argv) {
    auto app = Gtk::Application::create("org.gtkmm.example");

    return app->make_window_and_run<MainWindow>(argc, argv);
}