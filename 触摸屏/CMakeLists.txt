CMAKE_MINIMUM_REQUIRED(VERSION 3.10)

project(TCP)

set(PC ON)

if(PC)
    #设置编译器 （PC）
    set(CMAKE_C_COMPILER "/usr/bin/gcc")
    set(CMAKE_CXX_COMPILER "/usr/bin/g++")
else(PC)
    #设置交叉编译器 (开发板)
    set(CMAKE_C_COMPILER aarch64-linux-gnu-gcc)
    set(CMAKE_CXX_COMPILER aarch64-linux-gnu-g++)
endif(PC)
#设置编译属性
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g  -Wall")
set(CMAKE_BUILD_TYPE Debug)


if(PC)
         add_executable(TCP demo_01_client.cpp tcp.h tcp.cpp)      
else(PC)
        add_executable(TCP_S demo_02_server.cpp tcp.h tcp.cpp)
endif(PC)

