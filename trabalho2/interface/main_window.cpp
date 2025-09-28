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
		processedImage = img.clone();
		rightImagePanel.setImage(img); // show same initially
	});

	controlPanel.signal_apply_mirror().connect([this](bool horizontal) {
		ImageProcessor::mirrorImage(processedImage, horizontal);
		
		setOriginalImage(leftImagePanel);
		setProcessedImage(rightImagePanel);
	});

	controlPanel.signal_apply_contrast().connect([this](float contrastFactor) {
		ImageProcessor::transformImage(processedImage, [contrastFactor](cv::Vec3b& pixel) {
			ImageProcessor::contrastEnhancement(pixel, contrastFactor);
		});
		
		setOriginalImage(leftImagePanel);
		setProcessedImage(rightImagePanel);
	});

	controlPanel.signal_apply_grayscale().connect([this]() {
		ImageProcessor::transformImage(processedImage, ImageProcessor::toGrayScale);
		
		setOriginalImage(leftImagePanel);
		setProcessedImage(rightImagePanel);
	});

	controlPanel.signal_apply_quantize_gray().connect([this](int levels) {
		ImageProcessor::quantizeImage(processedImage, levels);
		
		setOriginalImage(leftImagePanel);
		setProcessedImage(rightImagePanel);
	});

	controlPanel.signal_apply_quantize_color().connect([this](int levels) {
		ImageProcessor::quantizeImageColored(processedImage, levels);
		
		setOriginalImage(leftImagePanel);
		setProcessedImage(rightImagePanel);
	});

	controlPanel.signal_create_histogram().connect([this]() {
		cv::Mat dummy = originalImage.clone();
		ImageProcessor::transformImage(dummy, ImageProcessor::toGrayScale);
		cv::Mat hist = ImageProcessor::computeHistogram(dummy);

		int hist_width = 512;
		int hist_height = 400;
		cv::Mat histImage = ImageProcessor::createHistogramImage(hist, hist_width, hist_height);
		
		setOriginalImage(leftImagePanel);

		rightImagePanel.setTitle("Histogram");
		rightImagePanel.setImage(histImage);
	});

	controlPanel.signal_create_processed_histogram().connect([this]() {
		cv::Mat dummy = processedImage.clone();
		ImageProcessor::transformImage(dummy, ImageProcessor::toGrayScale);
		cv::Mat hist = ImageProcessor::computeHistogram(dummy);

		int hist_width = 512;
		int hist_height = 400;
		cv::Mat histImage = ImageProcessor::createHistogramImage(hist, hist_width, hist_height);
		
		setProcessedImage(leftImagePanel);

		rightImagePanel.setTitle("Histogram");
		rightImagePanel.setImage(histImage);
	});

	controlPanel.signal_restart_image().connect([this]() {
		processedImage = originalImage.clone();
		
		setOriginalImage(leftImagePanel);
		setProcessedImage(rightImagePanel);
	});
}

void MainWindow::setOriginalImage(ImagePanel& panel) {
	panel.setTitle("Original Image");
	panel.setImage(originalImage);
}

void MainWindow::setProcessedImage(ImagePanel& panel) {
	panel.setTitle("Processed Image");
	panel.setImage(processedImage);
}

void MainWindow::updateProcessedImage() {
	if(processedImage.empty())
		processedImage = originalImage.clone();
}