#include "interface.h"
#include <gdkmm/pixbuf.h>
#include <gtkmm/filechooserdialog.h>
#include <giomm/file.h>
#include <iostream>

ImagePanel::ImagePanel(const std::string& title, bool canLoad) {
	set_label(title);
	set_margin(5);

	box.set_orientation(Gtk::Orientation::VERTICAL);
	set_child(box);

	box.append(label);
	box.append(image);

    if(canLoad) {
        box.append(btnLoad);

        btnLoad.signal_clicked().connect(sigc::mem_fun(*this, &ImagePanel::loadImageFromDialog));
    }

    image.set_hexpand(true);
    image.set_vexpand(true);

	label.set_text("No image loaded");
}

void ImagePanel::setImage(const cv::Mat& img) {
	if(img.empty()) {
		label.set_text("No image loaded");
		image.clear();
		return;
	}

	auto pixbuf = matToPixbuf(img);
    // pixbuf = pixbuf->scale_simple(targetWidth, targetHeight, Gdk::InterpType::BILINEAR);
	image.set(pixbuf);
	label.set_text("");
    label.hide();

    signalImageLoaded.emit(img.clone());
}

Glib::RefPtr<Gdk::Pixbuf> ImagePanel::matToPixbuf(const cv::Mat& mat) {
    cv::Mat m8;
    if (mat.depth() != CV_8U) {
        double minVal, maxVal;
        cv::minMaxLoc(mat, &minVal, &maxVal);
        double scale = (maxVal > minVal) ? 255.0/(maxVal-minVal) : 1.0;
        mat.convertTo(m8, CV_8U, scale, -minVal*scale);
    } else {
        m8 = mat;
    }

    cv::Mat rgb;
    switch (m8.channels()) {
        case 1:
            cv::cvtColor(m8, rgb, cv::COLOR_GRAY2RGB);
            break;
        
        case 3:
            cv::cvtColor(m8, rgb, cv::COLOR_BGR2RGB);
            break;

        case 4:
            cv::cvtColor(m8, rgb, cv::COLOR_BGRA2RGBA);
            break;

        default:
            throw std::runtime_error("Unsupported Mat channel count");
    }

    int w = rgb.cols, h = rgb.rows;
    bool has_alpha = (m8.channels() == 4);
    int pixbpp = has_alpha ? 4 : 3;

    auto pixbuf = Gdk::Pixbuf::create(Gdk::Colorspace::RGB, has_alpha, 8, w, h);
    guchar* dst = pixbuf->get_pixels();
    int stride = pixbuf->get_rowstride();
    for (int y = 0; y < h; ++y) {
        memcpy(dst + y*stride, rgb.ptr(y), w * pixbpp);
    }

    return pixbuf;
}

void ImagePanel::loadImageFromDialog() {
    auto dialog = new Gtk::FileChooserDialog("Select an image",
                                             Gtk::FileChooser::Action::OPEN);
    
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

    // Handle response asynchronously
    dialog->signal_response().connect([this, dialog](int response) {
        if (response == Gtk::ResponseType::OK) {
            auto file = dialog->get_file();
            if (file) {
                auto path = file->get_path();
                cv::Mat img = cv::imread(path);
                setImage(img);
            }
        }
        delete dialog;
    });

    dialog->show(); // present the dialog
}

sigc::signal<void (cv::Mat)> ImagePanel::signal_image_loaded() {
    return signalImageLoaded;
}