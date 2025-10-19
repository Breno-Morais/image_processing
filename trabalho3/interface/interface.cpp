#include "interface.h"
#include <thread>
#include <iostream>
#include <atomic>

GtkInterface::GtkInterface(VideoProcessor& processor) : processor(processor) {}

void GtkInterface::run() {
    // Launch GTK in a separate thread
    std::thread([this]() {
        // Initialize GTK threading support
        if (!g_thread_supported())
            g_thread_init(nullptr);
        gdk_threads_init();

        gtk_init(nullptr, nullptr);

        GtkWidget* window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
        gtk_window_set_title(GTK_WINDOW(window), "Video Controls");
        gtk_window_set_default_size(GTK_WINDOW(window), 300, 250);

        GtkWidget* vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
        gtk_container_add(GTK_CONTAINER(window), vbox);

        // Brightness slider
        GtkWidget* brightness_label = gtk_label_new("Brightness");
        GtkWidget* brightness_scale = gtk_scale_new_with_range(GTK_ORIENTATION_HORIZONTAL, -100, 100, 1);
        gtk_scale_set_value_pos(GTK_SCALE(brightness_scale), GTK_POS_RIGHT);
        g_signal_connect(brightness_scale, "value-changed", G_CALLBACK(on_brightness_changed), this);

        // Contrast slider
        GtkWidget* contrast_label = gtk_label_new("Contrast");
        GtkWidget* contrast_scale = gtk_scale_new_with_range(GTK_ORIENTATION_HORIZONTAL, 0, 200, 1);
        gtk_scale_set_value_pos(GTK_SCALE(contrast_scale), GTK_POS_RIGHT);
        gtk_range_set_value(GTK_RANGE(contrast_scale), 50);
        g_signal_connect(contrast_scale, "value-changed", G_CALLBACK(on_contrast_changed), this);

        // Blur slider
        GtkWidget* blur_label = gtk_label_new("Blur Kernel Size");
        GtkWidget* blur_scale = gtk_scale_new_with_range(GTK_ORIENTATION_HORIZONTAL, 1, 25, 2);
        gtk_scale_set_value_pos(GTK_SCALE(blur_scale), GTK_POS_RIGHT);
        g_signal_connect(blur_scale, "value-changed", G_CALLBACK(on_blur_changed), this);

        gtk_box_pack_start(GTK_BOX(vbox), brightness_label, FALSE, FALSE, 5);
        gtk_box_pack_start(GTK_BOX(vbox), brightness_scale, FALSE, FALSE, 5);
        gtk_box_pack_start(GTK_BOX(vbox), contrast_label, FALSE, FALSE, 5);
        gtk_box_pack_start(GTK_BOX(vbox), contrast_scale, FALSE, FALSE, 5);
        gtk_box_pack_start(GTK_BOX(vbox), blur_label, FALSE, FALSE, 5);
        gtk_box_pack_start(GTK_BOX(vbox), blur_scale, FALSE, FALSE, 5);

        g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

        gtk_widget_show_all(window);

        // GTK loop runs independently
        gdk_threads_enter();
        gtk_main();
        gdk_threads_leave();
    }).detach();
}

// Callbacks remain identical
void GtkInterface::on_brightness_changed(GtkRange* range, gpointer user_data) {
    auto* self = static_cast<GtkInterface*>(user_data);
    int value = static_cast<int>(gtk_range_get_value(range));
    self->processor.adjustParameter("brightness", value);
}

void GtkInterface::on_contrast_changed(GtkRange* range, gpointer user_data) {
    auto* self = static_cast<GtkInterface*>(user_data);
    int value = static_cast<int>(gtk_range_get_value(range));
    self->processor.adjustParameter("contrast", value);
}

void GtkInterface::on_blur_changed(GtkRange* range, gpointer user_data) {
    auto* self = static_cast<GtkInterface*>(user_data);
    int value = static_cast<int>(gtk_range_get_value(range));
    if (value % 2 == 0) value += 1;
    self->processor.adjustParameter("blur", value);
}
