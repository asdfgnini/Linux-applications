cmake_minimum_required(VERSION 3.10)
project(TCP)

set(CLIENT TCP)
set(SERVER TCP_S)

set(PC OFF)

if(PC)
    #设置编译器 （PC）
    set(CMAKE_C_COMPILER "/usr/bin/gcc")
    set(CMAKE_CXX_COMPILER "/usr/bin/g++")
else(PC)
    #设置交叉编译器 (开发板)
    set(CMAKE_C_COMPILER "/home/topeet/Downloads/gcc-linaro-6.3.1-2017.05-x86_64_aarch64-linux-gnu/bin/aarch64-linux-gnu-gcc")
    set(CMAKE_CXX_COMPILER "/home/topeet/Downloads/gcc-linaro-6.3.1-2017.05-x86_64_aarch64-linux-gnu/bin/aarch64-linux-gnu-g++")
endif(PC)
#设置编译属性
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g  -Wall")
set(CMAKE_BUILD_TYPE Debug)

aux_source_directory(. TCP_SOURCE_FILES)

if(PC)
add_library(${CLIENT} SHARED ${TCP_SOURCE_FILES})
else(PC)
add_library(${SERVER} SHARED ${TCP_SOURCE_FILES})
endif(PC)
