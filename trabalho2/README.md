For the compilation of this project you must first have installed cmake, make, libopencv-dev and GTK.

If you are using linux you can install them with these commands:

`sudo apt install make`

`sudo apt install cmake`

`sudo apt install libopencv-dev`

`sudo apt install libgtk-4-dev`

`sudo apt install libgtkmm-4.0-dev`

This one I'm not sure anymore but I did use it.
`pkg-config --cflags gtkmm-4.0`

To build the project you must create a build folder, after entering the build folder, you must use the cmake and make commands. For this process you can use these commands:

```bash
mkdir build
cd build
cmake ..
make
```

After this, in your build folder you will have the executable FPI2. You can run just as normal, if there is a problem with the Dropdown menu for the convolution (for example if you are using WSL2) you must run the executable with a specific setting:
`GDK_BACKEND=x11 ./FPI2`