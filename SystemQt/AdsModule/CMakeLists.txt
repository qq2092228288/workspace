cmake_minimum_required(VERSION 3.14)

project(AdsModule LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(LIBRARY_OUTPUT_PATH ../bin_x86_win32/release)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core)
find_package(Qt5 REQUIRED COMPONENTS SerialPort)

add_library(AdsModule SHARED
  adsmodule.h adsmodule.cpp
  adsmoduleprivate.h adsmoduleprivate.cpp
)

target_link_libraries(AdsModule PRIVATE Qt${QT_VERSION_MAJOR}::Core)
target_link_libraries(AdsModule PRIVATE Qt5::SerialPort)

target_compile_definitions(AdsModule PRIVATE ADSMODULE_LIBRARY)
