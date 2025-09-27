#include "interface.h"
#include <iostream>

MainWindow::MainWindow() {
    set_title("FPI");
    set_default_size(1200, 400);
    set_child(mainBox);

    mainBox.append(leftImagePanel);
    mainBox.append(rightImagePanel);

    // leftImagePanel.set_hexpand(true);
    // rightImagePanel.set_hexpand(true);

    // Load a test image
    cv::Mat img = cv::imread("../../test_images/Gramado_72k.jpg");
    if (!img.empty()) {
        leftImagePanel.setImage(img);
        rightImagePanel.setImage(img); // same for now
    }
}