CMAKE_MINIMUM_REQUIRED(VERSION 3.10)

project(TCP_S)

#库目录
set(TOP_DIR ${CMAKE_CURRENT_LIST_DIR}/../)


#设置编译器 （开发板）
set(CMAKE_C_COMPILER "aarch64-linux-gnu-gcc")
set(CMAKE_CXX_COMPILER "aarch64-linux-gnu-g++")

#设置编译属性
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g  -Wall")
set(CMAKE_BUILD_TYPE Debug)


add_executable(TCP_S demo_03_epoll_server.cpp ./Object/Object.h ./Object/Object.cpp)




