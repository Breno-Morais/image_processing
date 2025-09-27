cmake_minimum_required(VERSION 3.5)

project( FPI1 )

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_TYPE Debug)

find_package( OpenCV REQUIRED )
find_package(PkgConfig REQUIRED)
pkg_check_modules(GTKMM REQUIRED gtkmm-4.0)

include_directories( 
    ${OpenCV_INCLUDE_DIRS} 
    ${GTKMM_INCLUDE_DIRS}
)

add_executable( FPI1 main.cpp interface/image_panel.cpp interface/main_window.cpp interface/control_panel.cpp interface/interface.cpp )

target_link_libraries( FPI1 
    ${OpenCV_LIBS} 
    ${GTKMM_LIBRARIES}
)

target_compile_options(FPI1 PRIVATE ${GTKMM_CFLAGS_OTHER})

# cmake_minimum_required(VERSION 3.5)

# project( FPI1 )

# # Set the build type to Debug to include debugging symbols
# set(CMAKE_BUILD_TYPE Debug)

# find_package( OpenCV REQUIRED )
# find_package( PkgCongif REQUIRED)
# pkg_check_modules( GTK3 REQUIRED gtk+-3.0 )
# link_directories( ${GTK3_LIBRARY_DIRS} )
# add_compile_options(${GTK3_CFLAGS_OTHER})
# include_directories( ${OpenCV_INCLUDE_DIRS} )
# set(LIBRARIES ${LIBRARIES} ${GTK3_LIBRARIES} ${OpenCV_LIBS})

# set(FLAGS "-I${GTK3_INCLUDE_DIRS}")
# message(STATUS "Flags: ${FLAGS}")
# string(REPLACE ";" " -I" FLAGS "${FLAGS}")
# set(CMAKE_C_FLAGS ${CMAKE_C_FLAGS} ${GTK3_FLAGS} ${FLAGS})

# add_executable( FPI1 main.cpp )
# target_link_libraries( FPI1 ${LIBRARIES} )