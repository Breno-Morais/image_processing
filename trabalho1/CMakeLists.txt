cmake_minimum_required(VERSION 3.5)

project( FPI1 )

find_package( OpenCV REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} )
add_executable( FPI1 main.cpp )
target_link_libraries( FPI1 ${OpenCV_LIBS} )