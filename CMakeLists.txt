cmake_minimum_required (VERSION 2.6)
project (inventory CXX)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

# Find the QtWidgets library
find_package(Qt5Widgets)
find_package(Qt5Core)
find_package(Qt5Sql)


find_package(PkgConfig REQUIRED)
pkg_search_module(LIBUSB1 REQUIRED libusb-1.0)
include_directories(SYSTEM ${LIBUSB1_INCLUDE_DIRS})

pkg_search_module(LIBSQLITE3 REQUIRED sqlite3)
include_directories(SYSTEM ${LIBSQLITE3_INCLUDE_DIRS})

add_executable(inventory main.cpp mymodel.cpp)

set_property(TARGET inventory PROPERTY CXX_STANDARD 11)
set_property(TARGET inventory PROPERTY CXX_STANDARD_REQUIRED ON)


target_link_libraries(inventory Qt5::Widgets Qt5::Sql usb-1.0 sqlite3)



