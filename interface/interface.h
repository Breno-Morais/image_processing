#ifndef FPI_INTERFACE_H
#define FPI_INTERFACE_H

#include "./image/control_panel.h"
#include "./image/image_panel.h"

#include <gtkmm.h>
#include <opencv2/opencv.hpp>

#include <map>

class MainWindow: public Gtk::Window {
	public:
		MainWindow();

	protected:
		Gtk::Box mainBox{Gtk::Orientation::HORIZONTAL};

		Gtk::Box leftBox{Gtk::Orientation::VERTICAL};
		ImagePanel topLeftImagePanel{"Original Image", true};
		ImagePanel topRightImagePanel{"Processed Image", false};
		
		ControlPanel controlPanel;

		Gtk::Box rightBox{Gtk::Orientation::VERTICAL};
		ImagePanel bottomLeftImagePanel{"", false};
		ImagePanel bottomRightImagePanel{"", false};

		cv::Mat originalImage;
		cv::Mat processedImage;

		void setOriginalImage(ImagePanel& panel);
		void setProcessedImage(ImagePanel& panel);
		void updateProcessedImage();
};

#endif //FPI_INTERFACE_H