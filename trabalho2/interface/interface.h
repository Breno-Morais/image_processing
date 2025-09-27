#ifndef FPI_INTERFACE_H
#define FPI_INTERFACE_H

#include <gtkmm/window.h>
#include <gtkmm/frame.h>
#include <gtkmm/box.h>
#include <gtkmm/button.h>
#include <gtkmm/image.h>
#include <gtkmm/label.h>

#include <opencv2/opencv.hpp>

class ImagePanel: public Gtk::Frame {
	public:
		ImagePanel(const std::string& title);
		
		void setImage(const cv::Mat& img);
		
	protected:
		Gtk::Box box{Gtk::Orientation::VERTICAL};
		Gtk::Image image;
		Gtk::Label label;

		Glib::RefPtr<Gdk::Pixbuf> matToPixbuf(const cv::Mat& mat);
};

class MainWindow: public Gtk::Window {
	public:
		MainWindow();

		void loadLeftImage(const cv::Mat& img) { leftImagePanel.setImage(img); }
		void loadRightImage(const cv::Mat& img) { rightImagePanel.setImage(img); }

	protected:
		Gtk::Box mainBox{Gtk::Orientation::HORIZONTAL};
		ImagePanel leftImagePanel{"Original Image"};
		ImagePanel rightImagePanel{"Processed Image"};
};


class ControlPanel: public Gtk::Frame {
	public:
		ControlPanel();
		
		
		
	protected:
};

class HelloWorld: public Gtk::Window {
public:
	HelloWorld();
	~HelloWorld() override;

protected:
	//Signal handlers:
	void on_button_clicked();

	//Member widgets:
	Gtk::Button m_button;
};

#endif //FPI_INTERFACE_H