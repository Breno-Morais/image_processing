#include "interface.h"
#include "../image_processing/image_processing.h"
#include <iostream>

#include <gtkmm/scrolledwindow.h>

MainWindow::MainWindow() {
	set_title("FPI");
	set_default_size(1900, 400);
	set_child(mainBox);

	// Necessario pois a imagem quando screen demais dá erro
	Gtk::ScrolledWindow scroller1;
	scroller1.set_child(leftBox);
	mainBox.append(scroller1);

	leftBox.append(topLeftImagePanel);
	leftBox.append(bottomLeftImagePanel);

	mainBox.append(controlPanel);
	controlPanel.set_hexpand(false);
	mainBox.set_homogeneous(false);

	Gtk::ScrolledWindow scroller2;
	scroller2.set_child(rightBox);
	mainBox.append(scroller2);

	rightBox.append(topRightImagePanel);
	rightBox.append(bottomRightImagePanel);

	topLeftImagePanel.set_hexpand(true);
	topRightImagePanel.set_hexpand(true);
	controlPanel.set_hexpand(false);
	bottomLeftImagePanel.hide();
	bottomRightImagePanel.hide();

	topLeftImagePanel.signalImageLoaded.connect([this](cv::Mat img) {
		originalImage = img; // store original
		processedImage = img.clone();
		topRightImagePanel.setImage(img); // show same initially

		bottomLeftImagePanel.hide();
		bottomRightImagePanel.hide();
	});

	controlPanel.signalMirror.connect([this](bool horizontal) {
		ImageProcessor::mirrorImage(processedImage, horizontal);
		
		setOriginalImage(topLeftImagePanel);
		bottomLeftImagePanel.hide();
		
		setProcessedImage(topRightImagePanel);
		bottomRightImagePanel.hide();
	});

	controlPanel.signalContrast.connect([this](float contrastFactor) {
		ImageProcessor::transformImage(processedImage, [contrastFactor](cv::Vec3b& pixel) {
			ImageProcessor::contrastEnhancement(pixel, contrastFactor);
		});
		
		setOriginalImage(topLeftImagePanel);
		bottomLeftImagePanel.hide();
		
		setProcessedImage(topRightImagePanel);
		bottomRightImagePanel.hide();
	});

	controlPanel.signalGray.connect([this]() {
		ImageProcessor::transformImage(processedImage, ImageProcessor::toGrayScale);
		
		setOriginalImage(topLeftImagePanel);
		bottomLeftImagePanel.hide();
		
		setProcessedImage(topRightImagePanel);
		bottomRightImagePanel.hide();
	});

	controlPanel.signalQuantGray.connect([this](int levels) {
		ImageProcessor::quantizeImage(processedImage, levels);
		
		setOriginalImage(topLeftImagePanel);
		bottomLeftImagePanel.hide();
		
		setProcessedImage(topRightImagePanel);
		bottomRightImagePanel.hide();
	});

	controlPanel.signalQuantColor.connect([this](int levels) {
		ImageProcessor::quantizeImageColored(processedImage, levels);
		
		setOriginalImage(topLeftImagePanel);
		bottomLeftImagePanel.hide();
		
		setProcessedImage(topRightImagePanel);
		bottomRightImagePanel.hide();
	});

	controlPanel.signalHist.connect([this]() {
		cv::Mat dummy = originalImage.clone();
		ImageProcessor::transformImage(dummy, ImageProcessor::toGrayScale);
		cv::Mat hist = ImageProcessor::computeHistogram(dummy);

		int hist_width = 512;
		int hist_height = 400;
		cv::Mat histImage = ImageProcessor::createHistogramImage(hist, hist_width, hist_height, cv::Scalar(0, 0, 0));
		
		setOriginalImage(topLeftImagePanel);

		topRightImagePanel.setTitle("Histogram");
		topRightImagePanel.setImage(histImage);
	});

	controlPanel.signalProcessedHist.connect([this]() {
		cv::Mat dummy = processedImage.clone();
		ImageProcessor::transformImage(dummy, ImageProcessor::toGrayScale);
		cv::Mat hist = ImageProcessor::computeHistogram(dummy);

		int hist_width = 512;
		int hist_height = 400;
		cv::Mat histImage = ImageProcessor::createHistogramImage(hist, hist_width, hist_height, cv::Scalar(0, 0, 0));
		
		setProcessedImage(topLeftImagePanel);

		topRightImagePanel.setTitle("Histogram");
		topRightImagePanel.setImage(histImage);
	});

	controlPanel.signalBright.connect([this](int brightnessEnhancement) {
		ImageProcessor::transformImage(processedImage, [brightnessEnhancement](cv::Vec3b& pixel) {
			ImageProcessor::brightnessEnhancement(pixel, brightnessEnhancement);
		});
		
		setOriginalImage(topLeftImagePanel);
		bottomLeftImagePanel.hide();
		
		setProcessedImage(topRightImagePanel);
		bottomRightImagePanel.hide();
	});

	controlPanel.signalNeg.connect([this]() {		
		ImageProcessor::transformImage(processedImage, ImageProcessor::toNegative);

		setOriginalImage(topLeftImagePanel);
		bottomLeftImagePanel.hide();
		
		setProcessedImage(topRightImagePanel);
		bottomRightImagePanel.hide();
	});

	controlPanel.signalRestart.connect([this]() {
		processedImage = originalImage.clone();
		
		setOriginalImage(topLeftImagePanel);
		bottomLeftImagePanel.hide();
		
		setProcessedImage(topRightImagePanel);
		bottomRightImagePanel.hide();
	});

	controlPanel.signalEqHist.connect([this]() {
		cv::Mat dummy = originalImage.clone();
		ImageProcessor::transformImage(dummy, ImageProcessor::toGrayScale);
		cv::Mat hist = ImageProcessor::computeHistogram(dummy);

		int hist_width = 512;
		int hist_height = 400;
		cv::Mat histImage = ImageProcessor::createHistogramImage(hist, hist_width, hist_height, cv::Scalar(0, 0, 0));
		
		topLeftImagePanel.setTitle("Grey Original Image");
		topLeftImagePanel.setImage(dummy);
		
		bottomLeftImagePanel.setTitle("Histogram");
		bottomLeftImagePanel.setImage(histImage);
		bottomLeftImagePanel.show();
		
		cv::Mat eqHistImage;
		std::tie(processedImage, eqHistImage) = ImageProcessor::equalizeHistogram(dummy);

		setProcessedImage(topRightImagePanel);
		bottomRightImagePanel.setTitle("Equalized Histogram");
		bottomRightImagePanel.setImage(eqHistImage);	
		bottomRightImagePanel.show();
	});

	controlPanel.signalHistMatch.connect([this]() {
		auto dialog = new Gtk::FileChooserDialog("Select reference image", Gtk::FileChooser::Action::OPEN);

		if (auto window = dynamic_cast<Gtk::Window*>(get_root())) {
			dialog->set_transient_for(*window);
		}

		dialog->add_button("_Cancel", Gtk::ResponseType::CANCEL);
		dialog->add_button("_Open", Gtk::ResponseType::OK);

		auto filter = Gtk::FileFilter::create();
		filter->set_name("Images");
		filter->add_mime_type("image/jpeg");
		filter->add_mime_type("image/png");
		filter->add_mime_type("image/bmp");
		dialog->add_filter(filter);

		dialog->signal_response().connect([this, dialog](int response) {
			if (response == Gtk::ResponseType::OK) {
				auto file = dialog->get_file();
				if (file) {
					auto path = file->get_path();
					cv::Mat ref = cv::imread(path);

					if (!ref.empty()) {
						// converter ambas para grayscale (lembrando: suas imagens têm 3 canais iguais)
						cv::Mat srcGray, refGray;
						cv::cvtColor(processedImage, srcGray, cv::COLOR_BGR2GRAY);
						cv::cvtColor(ref, refGray, cv::COLOR_BGR2GRAY);

						ImageProcessor::HistMatchResult result = ImageProcessor::histogramMatching(srcGray, refGray, 256);

						// volta para 3 canais para manter compatibilidade
						processedImage = result.matchedImage;

						topLeftImagePanel.setTitle("Original Histogram");
						topLeftImagePanel.setImage(result.histSrcImage);

						bottomLeftImagePanel.setTitle("Reference Histogram");
						bottomLeftImagePanel.setImage(result.histRefImage);
						bottomLeftImagePanel.show();

						setProcessedImage(topRightImagePanel);
						bottomRightImagePanel.setTitle("Matched Histogram");
						bottomRightImagePanel.setImage(result.histMatchedImage);
						bottomRightImagePanel.show();
					}
				}
			}
			delete dialog;
		});

		dialog->show();
	});

	controlPanel.signalZoomIn.connect([this]() {
		processedImage = ImageProcessor::zoomIn(processedImage);

		setOriginalImage(topLeftImagePanel);
		bottomLeftImagePanel.hide();

		setProcessedImage(topRightImagePanel);
		bottomRightImagePanel.hide();
	});

	controlPanel.signalZoomOut.connect([this](int sx, int sy) {
		processedImage = ImageProcessor::zoomOut(processedImage, sx, sy);

		setOriginalImage(topLeftImagePanel);
		bottomLeftImagePanel.hide();

		setProcessedImage(topRightImagePanel);
		bottomRightImagePanel.hide();
	});

	controlPanel.signalRotate.connect([this](bool clockwise) {
		processedImage = ImageProcessor::rotateImage90(processedImage, clockwise);

		setOriginalImage(topLeftImagePanel);
		bottomLeftImagePanel.hide();

		setProcessedImage(topRightImagePanel);
		bottomRightImagePanel.hide();
	});

	controlPanel.signalConvolve.connect([this](cv::Mat kernel, bool luminanceOnly, bool add127) {
		processedImage = ImageProcessor::convolve(processedImage, kernel, luminanceOnly, add127);

		setOriginalImage(topLeftImagePanel);
		bottomLeftImagePanel.hide();

		setProcessedImage(topRightImagePanel);
		bottomRightImagePanel.hide();
	});

	controlPanel.signalSaveImage.connect([this]() {
		if (processedImage.empty()) return;

		auto dialog = new Gtk::FileChooserDialog("Save processed image",
												Gtk::FileChooser::Action::SAVE);

		if (auto window = dynamic_cast<Gtk::Window*>(get_root())) {
			dialog->set_transient_for(*window);
		}

		dialog->add_button("_Cancel", Gtk::ResponseType::CANCEL);
		dialog->add_button("_Save", Gtk::ResponseType::OK);

		auto filter = Gtk::FileFilter::create();
		filter->set_name("Images");
		filter->add_pattern("*.png");
		filter->add_pattern("*.jpg");
		filter->add_pattern("*.bmp");
		dialog->add_filter(filter);

		dialog->signal_response().connect([this, dialog](int response) {
			if (response == Gtk::ResponseType::OK) {
				auto file = dialog->get_file();
				if (file) {
					std::string path = file->get_path();
					try {
						// se não tiver extensão, salva como .png
						if (path.find('.') == std::string::npos) {
							path += ".png";
						}
						cv::imwrite(path, processedImage);
						std::cout << "Saved processed image to " << path << std::endl;
					} catch (const std::exception& e) {
						std::cerr << "Failed to save image: " << e.what() << std::endl;
					}
				}
			}
			delete dialog;
		});

		dialog->show();
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