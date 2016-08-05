cmake_minimum_required (VERSION 2.6)
project (tutorial)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)

# Find the QtWidgets library
find_package(Qt5Widgets)
find_package(Qt5Core)
find_package(Qt5Sql)

find_package(PkgConfig REQUIRED)
pkg_search_module(LIBUSB1 REQUIRED libusb-1.0)
include_directories(SYSTEM ${LIBUSB1_INCLUDE_DIRS})
  
add_executable(tutorial tutorial.cpp mymodel.cpp)

target_link_libraries(tutorial	Qt5::Widgets Qt5::Sql usb-1.0)



