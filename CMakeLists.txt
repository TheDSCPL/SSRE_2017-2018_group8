cmake_minimum_required(VERSION 3.8)
project(SSRE)

set(CMAKE_CXX_STANDARD 17)

set(SOURCE_FILES src/main.cpp src/Options.cpp headers/Options.hpp headers/exitCodes.hpp)
add_executable(SSRE ${SOURCE_FILES})