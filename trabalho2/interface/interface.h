#ifndef FPI_INTERFACE_H
#define FPI_INTERFACE_H

#include <gtkmm/window.h>
#include <gtkmm/frame.h>
#include <gtkmm/box.h>
#include <gtkmm/button.h>
#include <gtkmm/scale.h>
#include <gtkmm/spinbutton.h>
#include <gtkmm/image.h>
#include <gtkmm/label.h>

#include <sigc++/sigc++.h>

#include <opencv2/opencv.hpp>

class ImagePanel: public Gtk::Frame {
	public:
		ImagePanel(const std::string& title, bool canLoad);
		
		void setImage(const cv::Mat& img);
		
		sigc::signal<void (cv::Mat)> signal_image_loaded();

		protected:
		Gtk::Box box{Gtk::Orientation::VERTICAL};
		Gtk::Image image;
		Gtk::Label label;
		Gtk::Button btnLoad{"Load Image"};

		sigc::signal<void (cv::Mat)> signalImageLoaded;

		Glib::RefPtr<Gdk::Pixbuf> matToPixbuf(const cv::Mat& mat);
		void loadImageFromDialog();
		void setupDragAndDrop();
};

class ControlPanel: public Gtk::Frame {
	public:
		ControlPanel();

		sigc::signal<void (void)> signal_apply_grayscale();
		sigc::signal<void (bool)> signal_apply_mirror();
		sigc::signal<void (float)> signal_apply_contrast();
		sigc::signal<void (int)> signal_apply_quantize_gray();
		sigc::signal<void (int)> signal_apply_quantize_color();

	protected:
		Gtk::Box box{Gtk::Orientation::VERTICAL};

		Gtk::Button btnGray{"Grayscale"};
		Gtk::Button btnMirrorH{"Mirror Horizontal"};
		Gtk::Button btnMirrorV{"Mirror Vertical"};
		Gtk::Scale contrastSlider;
		Gtk::Button btnApplyContrast{"Apply Contrast"};

		Gtk::SpinButton spinQuantGray;
		Gtk::Button btnQuantGray{"Apply Quantize Gray"};

		Gtk::SpinButton spinQuantColor;
		Gtk::Button btnQuantColor{"Apply Quantize Color"};

		// Signal objects
		sigc::signal<void (void)> signalGray;
		sigc::signal<void (bool)> signalMirror;
		sigc::signal<void (float)> signalContrast;
		sigc::signal<void (int)> signalQuantGray;
		sigc::signal<void (int)> signalQuantColor;
};

class MainWindow: public Gtk::Window {
	public:
		MainWindow();

	protected:
		Gtk::Box mainBox{Gtk::Orientation::HORIZONTAL};
		ImagePanel leftImagePanel{"Original Image", true};
		ImagePanel rightImagePanel{"Processed Image", false};
		ControlPanel controlPanel;

		cv::Mat originalImage;
		cv::Mat processedImage;

		void updateProcessedImage();
};

#endif //FPI_INTERFACE_H