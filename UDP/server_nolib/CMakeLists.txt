CMAKE_MINIMUM_REQUIRED(VERSION 3.10)

project(UDP_S)


#设置编译器 （开发板）
set(CMAKE_C_COMPILER "aarch64-linux-gnu-gcc")
set(CMAKE_CXX_COMPILER "aarch64-linux-gnu-g++")

#设置编译属性
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g  -Wall")
set(CMAKE_BUILD_TYPE Debug)

include_directories(../udp)
link_directories(../udp/build)

add_executable(UDP_S demo_03_server.cpp)

target_link_libraries(UDP_S libUDP_S.so)


