cmake_minimum_required(VERSION 3.5)
project(qt_imageviewer LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt5 COMPONENTS Widgets REQUIRED)

message(status "found : ${Qt5Widgets_FOUND}")
message(status "found lib : Qt5::Widgets")
set(QT5_LIB ${Qt5Widgets_LIBRARIES})
set(QT5_INCLUDE_DIR ${Qt5Widgets_INCLUDE_DIRS})

find_package(OpenCV)
find_package(Boost COMPONENTS system log)

include_directories(${QT5_INCLUDE_DIR})
include_directories(${OpenCV_INCLUDE_DIRS})
include_directories(${Boost_INCLUDE_DIRS})
add_executable(imgviewer main.cpp zoomDialog.cpp qroiView.cpp viewport.cpp logger.cpp)
target_link_libraries(imgviewer ${OpenCV_LIBRARIES})
target_link_libraries(imgviewer ${Boost_LIBRARIES})
target_link_libraries(imgviewer Qt5::Widgets)

