#include "interface.h"
#include "../image_processing/image_processing.h"
#include <iostream>

MainWindow::MainWindow() {
	set_title("FPI");
	set_default_size(1200, 400);
	set_child(mainBox);

	mainBox.append(leftImagePanel);
	mainBox.append(controlPanel);
	mainBox.append(rightImagePanel);

	leftImagePanel.set_hexpand(true);
	rightImagePanel.set_hexpand(true);
	controlPanel.set_hexpand(false);

	leftImagePanel.signal_image_loaded().connect([this](cv::Mat img) {
		originalImage = img; // store original
		processedImage = img;
		rightImagePanel.setImage(img); // show same initially
	});

	controlPanel.signal_apply_mirror().connect([this](bool horizontal) {

		updateProcessedImage();
		ImageProcessor::mirrorImage(processedImage, horizontal);
		rightImagePanel.setImage(processedImage);
	});

	controlPanel.signal_apply_contrast().connect([this](float contrastFactor) {
		updateProcessedImage();
		ImageProcessor::transformImage(processedImage, [contrastFactor](cv::Vec3b& pixel) {
			ImageProcessor::contrastEnhancement(pixel, contrastFactor);
		});
		rightImagePanel.setImage(processedImage);
	});

	controlPanel.signal_apply_grayscale().connect([this]() {
		updateProcessedImage();
		ImageProcessor::transformImage(processedImage, ImageProcessor::toGrayScale);
		rightImagePanel.setImage(processedImage);
	});

	controlPanel.signal_apply_quantize_gray().connect([this](int levels) {
		updateProcessedImage();
		ImageProcessor::quantizeImage(processedImage, levels);
		rightImagePanel.setImage(processedImage);
	});

	controlPanel.signal_apply_quantize_color().connect([this](int levels) {
		updateProcessedImage();
		ImageProcessor::quantizeImageColored(processedImage, levels);
		rightImagePanel.setImage(processedImage);
	});

}

void MainWindow::updateProcessedImage() {
	if(processedImage.empty())
		processedImage = originalImage.clone();
}