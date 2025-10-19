#ifndef FPI_INTERFACE_H
#define FPI_INTERFACE_H

#include <gtk/gtk.h>
#include <opencv2/opencv.hpp>

#include "../core/video_processor.h"

class GtkInterface {
	public:
		GtkInterface(VideoProcessor& processor);
		void run();

	private:
		static void on_brightness_changed(GtkRange* range, gpointer user_data);
		static void on_contrast_changed(GtkRange* range, gpointer user_data);
		static void on_blur_changed(GtkRange* range, gpointer user_data);
		static void on_resize_changed(GtkRange* range, gpointer user_data);

		VideoProcessor& processor;
};

#endif //FPI_INTERFACE_H