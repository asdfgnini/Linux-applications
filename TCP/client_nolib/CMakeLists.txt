CMAKE_MINIMUM_REQUIRED(VERSION 3.10)

project(TCP)

#库目录
set(TOP_DIR ${CMAKE_CURRENT_LIST_DIR}/../)


#设置编译器 （PC）
set(CMAKE_C_COMPILER "/usr/bin/gcc")
set(CMAKE_CXX_COMPILER "/usr/bin/g++")

#设置编译属性
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g  -Wall")
set(CMAKE_BUILD_TYPE Debug)


include_directories(${TOP_DIR}/tcp)
link_directories(${TOP_DIR}/tcp/build)

add_executable(TCP demo_01_client.cpp )

target_link_libraries(TCP libTCP.so)



