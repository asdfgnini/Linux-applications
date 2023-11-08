CMAKE_MINIMUM_REQUIRED(VERSION 3.10)

project(TCP_S)


#设置编译器 （开发板）
set(CMAKE_C_COMPILER "aarch64-linux-gnu-gcc")
set(CMAKE_CXX_COMPILER "aarch64-linux-gnu-g++")

#设置编译属性
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g  -Wall")
set(CMAKE_BUILD_TYPE Debug)


include_directories(/home/topeet/tools/libevent-arm)

link_directories(/home/topeet/tools/libevent-arm)
# link_directories(/usr/lib/x86_64-linux-gnu)

add_executable(TCP_S demo_04_libevent_client.cpp)

target_link_libraries(TCP_S libevent.so)


