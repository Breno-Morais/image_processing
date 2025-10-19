#include "interface.h"

#include <giomm.h>

constexpr int baseMargin = 4;

ControlPanel::ControlPanel() {
    set_label("Controls");
    set_label_align(0.5f);
    set_margin(5);

    set_child(box);
    box.append(box1);
    box.append(box2);
    box1.set_spacing(baseMargin);
    box1.set_margin(baseMargin);
    box2.set_spacing(baseMargin);
    box2.set_margin(baseMargin);

    box2.set_hexpand(false);
    box2.set_vexpand(false);

    appendGrayscale();

    appendMirror();

    appendContrast();

    appendQuantGray();

    appendQuantColor();
    
    appendNeg();
    
    appendBright();
    
    appendHistogram();

    appendProcHist();

    appendEqHist();

    appendHistMatch();

    appendZoom();

    appendRotate();

    initialize_presets();

    appendDropdown();

    appendRestart();

    appendSave();
}

void ControlPanel::initialize_presets() {
    kernelPresets["Gaussian (blur)"] = {
        {{{0.0625, 0.125, 0.0625},
          {0.125,  0.25,  0.125},
          {0.0625, 0.125, 0.0625}}},
        /*useLuminance=*/false,
        /*add127=*/false
    };

    kernelPresets["Laplacian"] = {
        {{{0.0, -1.0, 0.0},
          {-1.0,  4,  -1.0},
          {0.0, -1.0, 0.0}}},
        /*useLuminance=*/true,
        /*add127=*/false
    };

    kernelPresets["Passa Alta"] = {
        {{{-1, -1, -1},
          {-1,  8, -1},
          {-1, -1, -1}}},
        /*useLuminance=*/true,
        /*add127=*/false
    };

    kernelPresets["Prewitt Hx"] = {
        {{{-1, 0, 1},
          {-1, 0, 1},
          {-1, 0, 1}}},
        /*useLuminance=*/true,
        /*add127=*/true
    };

    kernelPresets["Prewitt Hy"] = {
        {{{-1, -1, -1},
          { 0,  0,  0},
          { 1,  1,  1}}},
        /*useLuminance=*/true,
        /*add127=*/true
    };

    kernelPresets["Sobel Hx"] = {
        {{{-1, 0, 1},
          {-2, 0, 2},
          {-1, 0, 1}}},
        /*useLuminance=*/true,
        /*add127=*/true
    };

    kernelPresets["Sobel Hy"] = {
        {{{-1, -2, -1},
          { 0,  0,  0},
          { 1,  2,  1}}},
        /*useLuminance=*/true,
        /*add127=*/true
    };
}

void ControlPanel::setKernel(const std::array<std::array<double,3>,3>& values) {
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            kernelEntries[i][j].set_text(std::to_string(values[i][j]));
        }
    }
}

void ControlPanel::appendGrayscale() {
    box1.append(btnGray);
    btnGray.signal_clicked().connect([this]() {
        signalGray.emit();
    });
}

void ControlPanel::appendMirror() {
    box1.append(btnMirrorH);
    btnMirrorH.signal_clicked().connect([this]() {
        signalMirror.emit(true);
    });

    box1.append(btnMirrorV);
    btnMirrorV.signal_clicked().connect([this]() {
        signalMirror.emit(false);
    });
}

void ControlPanel::appendContrast() {
    Gtk::Frame* frame = Gtk::make_managed<Gtk::Frame>("Contrast");
    frame->set_label_align(0.5f);
    Gtk::Box* _box = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::VERTICAL, 6);
    _box->set_margin(baseMargin);
    frame->set_child(*_box);

    contrastSlider.set_orientation(Gtk::Orientation::HORIZONTAL);
    contrastSlider.set_range(0.1, 3.0);
    contrastSlider.set_value(1.0);
    contrastSlider.set_increments(0.1, 0.5);
    contrastSlider.set_draw_value(true);
    contrastSlider.set_value_pos(Gtk::PositionType::RIGHT);
    
    btnApplyContrast.signal_clicked().connect([this]() {
        signalContrast.emit(contrastSlider.get_value());
    });
    
    _box->append(contrastSlider);
    _box->append(btnApplyContrast);
    box1.append(*frame);
}

void ControlPanel::appendQuantGray() {
    Gtk::Frame* frame = Gtk::make_managed<Gtk::Frame>("Quantatization Gray");
    frame->set_label_align(0.5f);
    Gtk::Box* _box = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::VERTICAL, 6);
    _box->set_margin(baseMargin);
    frame->set_child(*_box);

    spinQuantGray.set_range(2, 32);
    spinQuantGray.set_increments(1, 2);
    spinQuantGray.set_value(4);
    btnQuantGray.signal_clicked().connect([this]() {
        signalQuantGray.emit(spinQuantGray.get_value_as_int());
    });

    _box->append(spinQuantGray);
    _box->append(btnQuantGray);
    box1.append(*frame);
}

void ControlPanel::appendQuantColor() {
    Gtk::Frame* frame = Gtk::make_managed<Gtk::Frame>("Quantatization Gray");
    frame->set_label_align(0.5f);
    Gtk::Box* _box = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::VERTICAL, 6);
    _box->set_margin(baseMargin);
    frame->set_child(*_box);

    spinQuantColor.set_range(2, 32);
    spinQuantColor.set_increments(1, 2);
    spinQuantColor.set_value(4);
    btnQuantColor.signal_clicked().connect([this]() {
        signalQuantColor.emit(spinQuantColor.get_value_as_int());
    });

    _box->append(spinQuantColor);
    _box->append(btnQuantColor);
    box1.append(*frame);
}

void ControlPanel::appendHistogram() {
    box1.append(btnHist);
    btnHist.signal_clicked().connect([this]() {
        signalHist.emit();
    });
}

void ControlPanel::appendNeg() {
    box1.append(btnNeg);
    btnNeg.signal_clicked().connect([this]() {
        signalNeg.emit();
    });
}

void ControlPanel::appendBright() {
    Gtk::Frame* frame = Gtk::make_managed<Gtk::Frame>("Brightness");
    frame->set_label_align(0.5f);
    Gtk::Box* _box = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::VERTICAL, 6);
    _box->set_margin(baseMargin);
    frame->set_child(*_box);

    spinBright.set_range(-255, 255);
    spinBright.set_increments(1, 2);
    spinBright.set_value(0);
    btnBright.signal_clicked().connect([this]() {
        signalBright.emit(spinBright.get_value_as_int());
    });

    _box->append(spinBright);
    _box->append(btnBright);
    box1.append(*frame);
}

void ControlPanel::appendProcHist() {
    box1.append(btnProcHist);
    btnProcHist.signal_clicked().connect([this]() {
        signalProcessedHist.emit();
    });
}

void ControlPanel::appendEqHist() {
    box2.append(btnEqHist);
    btnEqHist.signal_clicked().connect([this]() {
        signalEqHist.emit();
    });
}

void ControlPanel::appendHistMatch() {
    box2.append(btnHistMatch);
    btnHistMatch.signal_clicked().connect([this]() {
        signalHistMatch.emit();
    });
}

void ControlPanel::appendZoom() {
    Gtk::Frame* frame = Gtk::make_managed<Gtk::Frame>("Zoom");
    frame->set_label_align(0.5f);
    Gtk::Box* _box = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::VERTICAL, 6);
    _box->set_margin(baseMargin);
    frame->set_child(*_box);
    
    btnZoomIn.signal_clicked().connect([this]() {
        signalZoomIn.emit();
    });

    Gtk::Box* _box2 = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL, 6);

    zoomOutX.set_text("2");
    zoomOutX.set_max_width_chars(5);

    zoomOutY.set_text("2");
    zoomOutY.set_max_width_chars(5);

    btnZoomOut.signal_clicked().connect([this]() {
        int x, y;
        try {
            x = std::stod(zoomOutX.get_text());
            y = std::stod(zoomOutY.get_text());
        } catch (...) {
            x = y = 2;
        }
        signalZoomOut.emit(x, y);
    });

    _box2->set_valign(Gtk::Align::CENTER);
    _box2->set_halign(Gtk::Align::CENTER);

    auto labelZoomOut = Gtk::make_managed<Gtk::Label>("Factor only for Zoom Out");
    auto labelX = Gtk::make_managed<Gtk::Label>("X:");
    auto labelY = Gtk::make_managed<Gtk::Label>("Y:");
    labelZoomOut->set_valign(Gtk::Align::CENTER);
    labelZoomOut->set_halign(Gtk::Align::CENTER);

    _box2->append(*labelX);
    _box2->append(zoomOutX);
    _box2->append(*labelY);
    _box2->append(zoomOutY);

    _box->append(btnZoomIn);
    _box->append(*labelZoomOut);
    _box->append(*_box2);
    _box->append(btnZoomOut);
    box2.append(*frame);
}

void ControlPanel::appendRotate() {
    box2.append(btnRotateC);
    btnRotateC.signal_clicked().connect([this]() {
        signalRotate.emit(true);
    });

    box2.append(btnRotateCC);
    btnRotateCC.signal_clicked().connect([this]() {
        signalRotate.emit(false);
    });
}

void ControlPanel::appendDropdown() {
    std::vector<Glib::ustring> kernelNames;
    kernelNames.push_back("Custom");
    for (const auto& pair : kernelPresets) {
        kernelNames.push_back(pair.first);
    }

    auto kernels = Gtk::StringList::create(kernelNames);
    dropdownKernels.set_model(kernels);
    dropdownKernels.set_selected(0); // Default to "Custom"

    // This is the key part: the signal handler
    dropdownKernels.property_selected().signal_changed().connect(
        [this]() {
            auto selected_item = dropdownKernels.get_selected_item();
            if (!selected_item) return;

            auto string_object = std::dynamic_pointer_cast<Gtk::StringObject>(selected_item);
            if (!string_object) return;
            
            Glib::ustring selected_kernel_name = string_object->get_string();

            if (selected_kernel_name == "Custom") {
                return;
            }

            try {
                const auto& preset = kernelPresets.at(selected_kernel_name);
                
                setKernel(preset.matrix);
                checkLuminance.set_active(preset.useLuminance);
                checkAdd127.set_active(preset.add127);

            } catch (const std::out_of_range& e) {
                std::cerr << "Error: Selected kernel not found in presets map: "
                          << selected_kernel_name << std::endl;
            }
    });

    // ---------- Grid 3x3 ----------
    kernelGrid.set_row_spacing(4);
    kernelGrid.set_column_spacing(4);
    kernelGrid.set_margin(10);
    kernelGrid.set_hexpand(false);

    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            kernelEntries[i][j].set_text("0.0");
            kernelEntries[i][j].set_max_width_chars(5);
            kernelGrid.attach(kernelEntries[i][j], j, i, 1, 1);
            kernelEntries[i][j].set_size_request(20,20);
        }
    }

    // ---------- Options ----------
    checkLuminance.set_active(true);
    checkAdd127.set_active(false);

    // ---------- Append ----------
    Gtk::Frame* frame = Gtk::make_managed<Gtk::Frame>("Kernel");
    frame->set_label_align(0.5f);
    Gtk::Box* _box = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::VERTICAL, 6);
    _box->append(dropdownKernels);
    _box->append(kernelGrid);
    _box->set_margin(baseMargin);
    frame->set_child(*_box);

    Gtk::Box* optionsBox = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::VERTICAL, 6);
    auto option_label = Gtk::make_managed<Gtk::Label>("Options");
    option_label->set_wrap(false);

    optionsBox->append(*option_label);
    optionsBox->append(checkLuminance);
    optionsBox->append(checkAdd127);

    _box->append(*optionsBox);
    _box->append(btnApplyKernel);
    box2.append(*frame);

    // ---------- Apply button ----------
    btnApplyKernel.signal_clicked().connect([this]() {
        cv::Mat kernel(3, 3, CV_64F);
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                try {
                    kernel.at<double>(i,j) = std::stod(kernelEntries[i][j].get_text());
                } catch (...) {
                    kernel.at<double>(i,j) = 0.0;
                }
            }
        }

        bool luminanceOnly = checkLuminance.get_active();
        bool add127 = checkAdd127.get_active();

        signalConvolve.emit(kernel, luminanceOnly, add127);
    });
}

void ControlPanel::appendRestart() {
    box1.append(btnRestart);
    btnRestart.signal_clicked().connect([this]() {
        signalRestart.emit();
    });
}

void ControlPanel::appendSave() {
    box2.append(btnSave);
    btnSave.signal_clicked().connect([this]() {
        signalSaveImage.emit();
    });
}