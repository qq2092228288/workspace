cmake_minimum_required(VERSION 3.14)

project(ZeYaoTebco LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(LIBRARY_OUTPUT_PATH /home/ubuntu/zeyaotebco/lib)

add_library(ZeYaoTebco SHARED
  zeyaotebco.h zeyaotebco.cpp
  zeyaotebcoprivate.h zeyaotebcoprivate.cpp
  datacalculation.h datacalculation.cpp
  reportdatajson.h reportdatajson.cpp
)

target_link_libraries(ZeYaoTebco paho-mqtt3as)
target_link_libraries(ZeYaoTebco paho-mqttpp3)
target_link_libraries(ZeYaoTebco cserialport)
target_link_libraries(ZeYaoTebco jsoncpp)

target_compile_definitions(ZeYaoTebco PRIVATE ZEYAOTEBCO_LIBRARY)
