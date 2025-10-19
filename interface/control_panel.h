#ifndef CONTROL_PANEL_H
#define CONTROL_PANEL_H

#include <gtkmm.h>
#include <sigc++/sigc++.h>
#include <opencv2/opencv.hpp>
#include <array>

using KernelMatrix = std::array<std::array<double, 3>, 3>;

struct KernelPreset {
    KernelMatrix matrix;
    bool useLuminance;
    bool add127;
};

class ControlPanel: public Gtk::Frame {
	public:
		ControlPanel();

		sigc::signal<void (void)> signalGray;
		sigc::signal<void (bool)> signalMirror;
		sigc::signal<void (float)> signalContrast;
		sigc::signal<void (int)> signalQuantGray;
		sigc::signal<void (int)> signalQuantColor;
		sigc::signal<void (void)> signalHist;
		sigc::signal<void (void)> signalEqHist;
		sigc::signal<void (void)> signalHistMatch;
		sigc::signal<void (void)> signalProcessedHist;
		sigc::signal<void (int)> signalBright;
		sigc::signal<void (void)> signalNeg;
		sigc::signal<void (void)> signalRestart;

		sigc::signal<void (void)> signalZoomIn;
		sigc::signal<void (int,int)> signalZoomOut;

		sigc::signal<void (bool)> signalRotate;

		sigc::signal<void (cv::Mat, bool, bool)> signalConvolve;
		sigc::signal<void (void)> signalSaveImage;

	protected:
		std::map<Glib::ustring, KernelPreset> kernelPresets;	
		
		Gtk::Box box{Gtk::Orientation::HORIZONTAL};
		Gtk::Box box1{Gtk::Orientation::VERTICAL};
		Gtk::Box box2{Gtk::Orientation::VERTICAL};

		Gtk::Button btnGray{"Grayscale"};
		Gtk::Button btnMirrorH{"Mirror Horizontal"};
		Gtk::Button btnMirrorV{"Mirror Vertical"};
		Gtk::Scale contrastSlider;
		Gtk::Button btnApplyContrast{"Apply Contrast"};

		Gtk::SpinButton spinQuantGray;
		Gtk::Button btnQuantGray{"Apply Quantize Gray"};

		Gtk::SpinButton spinQuantColor;
		Gtk::Button btnQuantColor{"Apply Quantize Color"};

		Gtk::Button btnHist{"Create Histogram"};
		Gtk::Button btnProcHist{"Create Processed Histogram"};
		Gtk::Button btnEqHist{"Equalize Histogram"};
		Gtk::Button btnHistMatch{"Match Histograms"};

		Gtk::SpinButton spinBright;
		Gtk::Button btnBright{"Apply Brightness Enhancement"};

		Gtk::Button btnNeg{"Apply Negative"};
		Gtk::Button btnRestart{"Restore Image"};
		
		Gtk::Button btnZoomIn{"Zoom In"};

		Gtk::Entry zoomOutX;
		Gtk::Entry zoomOutY;
		Gtk::Button btnZoomOut{"Zoom Out"};

		Gtk::Button btnRotateC{"Rotate Clockwise"};
		Gtk::Button btnRotateCC{"Rotate Counter-Clockwise"};

		Gtk::Grid kernelGrid;
		Gtk::Entry kernelEntries[3][3];
		Gtk::DropDown dropdownKernels;
		Gtk::Button btnApplyKernel{"Apply Convolution"};
		Gtk::CheckButton checkLuminance{"Apply only to luminance"};
		Gtk::CheckButton checkAdd127{"Add +127 before clamping"};

		Gtk::Button btnSave{"Save Image"};

		void initialize_presets();
		void setKernel(const std::array<std::array<double,3>,3>& values);

		void appendGrayscale();
		void appendMirror();
		void appendContrast();
		void appendQuantGray();
		void appendQuantColor();
		void appendHistogram();
		void appendNeg();
		void appendBright();
		void appendProcHist();
		void appendEqHist();
		void appendHistMatch();
		void appendZoom();
		void appendRotate();
		void appendDropdown();
		void appendRestart();
		void appendSave();
};

#endif /* CONTROL_PANEL_H */