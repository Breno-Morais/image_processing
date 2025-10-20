cmake_minimum_required(VERSION 3.5)

project( FPI3 )

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_TYPE Debug)

find_package( OpenCV REQUIRED )
find_package(PkgConfig REQUIRED)
pkg_check_modules(GTKMM REQUIRED gtkmm-3.0)

include_directories( 
    ${OpenCV_INCLUDE_DIRS} 
    ${GTKMM_INCLUDE_DIRS}
)

file(GLOB SRC_FILES
    "${CMAKE_SOURCE_DIR}/interface/*.cpp"
    "${CMAKE_SOURCE_DIR}/core/*.cpp"
    "${CMAKE_SOURCE_DIR}/*.cpp"
)

add_executable( FPI3 main.cpp  ${SRC_FILES} )

target_link_libraries( FPI3 
    ${OpenCV_LIBS} 
    ${GTKMM_LIBRARIES}
    pthread
)

target_compile_options(FPI3 PRIVATE ${GTKMM_CFLAGS_OTHER})