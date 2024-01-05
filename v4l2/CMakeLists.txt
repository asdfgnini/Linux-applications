CMAKE_MINIMUM_REQUIRED(VERSION 3.16)
project(V4l2)

set (CMAKE_C_COMPILER "arm-linux-gnueabihf-gcc")
set (CMAKE_CXX_COMPILER "arm-linux-gnueabihf-g++")


include_directories(/home/alientek/sdk/rv1126/buildroot/output/alientek_rv1126/build/libdrm-2.4.89)
include_directories(/home/alientek/sdk/rv1126/buildroot/output/alientek_rv1126/build/libdrm-2.4.89/include/drm)
include_directories(/home/alientek/sdk/rv1126/buildroot/output/alientek_rv1126/build/jpeg-turbo-2.0.2)



link_directories(/home/alientek/sdk/rv1126/buildroot/output/alientek_rv1126/target/usr/lib)

add_executable(V4l2 main.c)

target_link_libraries(V4l2 libdrm.so libjpeg.so)