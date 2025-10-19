#ifndef IMAGE_PANEL_H
#define IMAGE_PANEL_H

#include <gtkmm.h>
#include <sigc++/sigc++.h>
#include <opencv2/opencv.hpp>

class ImagePanel: public Gtk::Frame {
	public:
		ImagePanel(const std::string& title, bool canLoad);
		
		void setTitle(const std::string& title);
		void setImage(const cv::Mat& img, bool emitSignal = false);
		
		sigc::signal<void (cv::Mat)> signalImageLoaded;

		protected:
		Gtk::Box box{Gtk::Orientation::VERTICAL};
		Gtk::Image image;
		Gtk::Label label;
		Gtk::Button btnLoad{"Load Image"};

		Glib::RefPtr<Gdk::Pixbuf> matToPixbuf(const cv::Mat& mat);
		void loadImageFromDialog();
		void setupDragAndDrop();
};

#endif /* IMAGE_PANEL_H */