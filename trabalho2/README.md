For the compilation of this project you must first have installed cmake, make and libopencv-dev

If you are using linux you can install them with these commands:

`sudo apt install make`

`sudo apt install cmake`

`sudo apt install libopencv-dev`

To build the project you must create a build folder, after entering the build folder, you must use the cmake and make commands. For this process you can use these commands:

```bash
mkdir build
cd build
cmake ..
make
```

After this, in your build folder you will have the executable FPI1. You must execute it with the image files paths in the arguments of the program.