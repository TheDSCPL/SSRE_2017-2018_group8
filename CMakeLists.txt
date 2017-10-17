cmake_minimum_required(VERSION 3.8)
project(SSRE)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -Wall -pedantic")

set(SOURCE_FILES src/main.cpp src/Options.cpp headers/Options.hpp headers/exitCodes.hpp src/Process.cpp headers/Process.hpp src/Thread.cpp headers/Thread.hpp headers/Constants.hpp src/Utils.cpp headers/Utils.hpp)
add_executable(SSRE ${SOURCE_FILES})