#include "interface.h"
#include <iostream>

ImagePanel::ImagePanel(const std::string& title) {
	set_label(title);
	set_margin(5);

	box.set_orientation(Gtk::Orientation::VERTICAL);
	set_child(box);

	box.append(label);
	box.append(image);
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