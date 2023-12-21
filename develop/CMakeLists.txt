CMAKE_MINIMUM_REQUIRED(VERSION 3.16)
project(2k1000)


# set (CMAKE_C_COMPILER "mips64el-linux-gcc")
# set (CMAKE_CXX_COMPILER "mips64el-linux-g++")

set (CMAKE_C_COMPILER "aarch64-none-linux-gnu-gcc")
set (CMAKE_CXX_COMPILER "aarch64-none-linux-gnu-g++")

set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pthread" )



add_executable(2k1000 main.cpp 
            ./HotPlug.cpp 
            ./HotPlug.h
)