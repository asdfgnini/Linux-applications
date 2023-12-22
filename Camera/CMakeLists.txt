cmake_minimum_required(VERSION 3.16)

project(Camera)

# set (CMAKE_C_COMPILER "aarch64-none-linux-gnu-gcc")
# set (CMAKE_CXX_COMPILER "aarch64-none-linux-gnu-g++")

set (CMAKE_C_COMPILER "gcc")
set (CMAKE_CXX_COMPILER "g++")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pthread" )

add_executable(Camera 
        main.cpp
        v4l2.cpp
        v4l2.h)

