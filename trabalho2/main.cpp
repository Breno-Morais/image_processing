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

// int main(int argc, char** argv) {
//     if (argc < 2) {
//         std::cerr << "Usage: " << argv[0] << " <image_path1> <image_path2> ..." << std::endl;
//         return -1;
//     }

//     std::cout << "Choose an operation to perform on the images:\n";
//     std::cout << static_cast<int>(ImageOperation::MIRROR) << ". Mirror the image\n";
//     std::cout << static_cast<int>(ImageOperation::TO_GRAYSCALE) << ". Convert to Grayscale\n";
//     std::cout << static_cast<int>(ImageOperation::CONTRAST_ENHANCEMENT) << ". Enhance Contrast\n";
//     std::cout << static_cast<int>(ImageOperation::QUANTIZE_GRAYSCALE) << ". Quantize (Grayscale)\n";
//     std::cout << static_cast<int>(ImageOperation::QUANTIZE_COLOR) << ". Quantize (Color)\n";
//     std::cout << "Enter your choice: ";

//     int choice;
//     std::cin >> choice;

//     for (int i = 1; i < argc; ++i) {
//         cv::Mat image = cv::imread(argv[i], cv::IMREAD_COLOR);
//         cv::Mat originalImage = image.clone();

//         if (!image.data) {
//             std::cerr << "Could not open or find the image: " << argv[i] << std::endl;
//             continue;
//         }

//         switch (static_cast<ImageOperation>(choice)) {
//             case ImageOperation::MIRROR: {
//                 std::cout << "Mirror horizontally (1) or vertically (0): ";
//                 bool horizontal;
//                 std::cin >> horizontal;
//                 mirrorImage(image, horizontal);
//                 break;
//             }
//             case ImageOperation::TO_GRAYSCALE:
//                 transformImage(image, toGrayScale);
//                 break;
//             case ImageOperation::CONTRAST_ENHANCEMENT: {
//                 float contrastFactor;
//                 std::cout << "Enter contrast factor: ";
//                 std::cin >> contrastFactor;
//                 transformImage(image, [contrastFactor](cv::Vec3b& pixel) {
//                     contrastEnhancement(pixel, contrastFactor);
//                 });
//                 break;
//             }
//             case ImageOperation::QUANTIZE_GRAYSCALE: {
//                 int n;
//                 std::cout << "Enter number of grayscale levels: ";
//                 std::cin >> n;
//                 quantizeImage(image, n);
//                 break;
//             }
//             case ImageOperation::QUANTIZE_COLOR: {
//                 int n;
//                 std::cout << "Enter number of color levels: ";
//                 std::cin >> n;
//                 quantizeImageColored(image, n);
//                 break;
//             }
//             default:
//                 std::cerr << "Invalid choice." << std::endl;
//                 continue;
//         }

//         std::cout << "Do you want to save the processed image? (y/n): ";
//         char saveChoice;
//         std::cin >> saveChoice;

//         if (saveChoice == 'y' || saveChoice == 'Y') {
//             std::string outputPath;
//             std::cout << "Enter the output file path (it will be saved as a jpg): ";
//             std::cin >> outputPath;

//             outputPath += ".jpg";

//             if (cv::imwrite(outputPath, image)) {
//                 std::cout << "Image saved successfully to " << outputPath << std::endl;
//             } else {
//                 std::cerr << "Failed to save the image. Please check the file path and permissions." << std::endl;
//             }
//         }

//         int padding = 10; // Space between the images
//         cv::copyMakeBorder(originalImage, originalImage, 0, 0, 0, padding, cv::BORDER_CONSTANT, cv::Scalar(0, 0, 0));

//         cv::Mat combinedImage;
//         cv::hconcat(originalImage, image, combinedImage);

//         std::string windowName = "Original and Processed Image - " + std::to_string(i);
//         cv::namedWindow(windowName, cv::WINDOW_NORMAL);
//         cv::resizeWindow(windowName, combinedImage.cols, combinedImage.rows);
//         cv::imshow(windowName, combinedImage);
//     }

//     cv::waitKey(0);
//     return 0;
// }