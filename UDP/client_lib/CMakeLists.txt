CMAKE_MINIMUM_REQUIRED(VERSION 3.10)

project(UDP)

#设置编译器 （PC）
set(CMAKE_C_COMPILER "/usr/bin/gcc")
set(CMAKE_CXX_COMPILER "/usr/bin/g++")


#设置编译属性
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g  -Wall")
set(CMAKE_BUILD_TYPE Debug)


include_directories(/home/topeet/tools/libevent)

link_directories(/home/topeet/tools/libevent)
# link_directories(/usr/lib/x86_64-linux-gnu)

add_executable(UDP demo_03_libevent_client.cpp)


target_link_libraries(UDP libevent.a libevent_core.a libevent_extra.a libevent_openssl.a libevent_pthreads.a)


