#include "interface.h"

ControlPanel::ControlPanel() {
    set_label("Controls");
    set_margin(5);

    set_child(box);

    // Grayscale
    box.append(btnGray);
    btnGray.signal_clicked().connect([this]() {
        signalGray.emit();
    });

    // Mirror
    box.append(btnMirrorH);
    btnMirrorH.signal_clicked().connect([this]() {
        signalMirror.emit(true);
    });

    box.append(btnMirrorV);
    btnMirrorV.signal_clicked().connect([this]() {
        signalMirror.emit(false);
    });

    // Contrast slider
    contrastSlider.set_orientation(Gtk::Orientation::HORIZONTAL);
    contrastSlider.set_range(0.1, 3.0);
    contrastSlider.set_value(1.0);
    contrastSlider.set_increments(0.1, 0.5);

    box.append(contrastSlider);
    box.append(btnApplyContrast);
    btnApplyContrast.signal_clicked().connect([this]() {
        signalContrast.emit(contrastSlider.get_value());
    });

    // Quantize (Gray)
    spinQuantGray.set_range(2, 32);
    spinQuantGray.set_increments(1, 2);
    spinQuantGray.set_value(4);
    box.append(spinQuantGray);
    box.append(btnQuantGray);
    btnQuantGray.signal_clicked().connect([this]() {
        signalQuantGray.emit(spinQuantGray.get_value_as_int());
    });

    // Quantize (Color)
    spinQuantColor.set_range(2, 32);
    spinQuantColor.set_increments(1, 2);
    spinQuantColor.set_value(4);
    box.append(spinQuantColor);
    box.append(btnQuantColor);
    btnQuantColor.signal_clicked().connect([this]() {
        signalQuantColor.emit(spinQuantColor.get_value_as_int());
    });
}


sigc::signal<void (void)> ControlPanel::signal_apply_grayscale() { return signalGray; }
sigc::signal<void (bool)> ControlPanel::signal_apply_mirror() { return signalMirror; }
sigc::signal<void (float)> ControlPanel::signal_apply_contrast() { return signalContrast; }
sigc::signal<void (int)> ControlPanel::signal_apply_quantize_gray() { return signalQuantGray; }
sigc::signal<void (int)> ControlPanel::signal_apply_quantize_color() { return signalQuantColor; }