CMAKE_MINIMUM_REQUIRED(VERSION 3.16)
project(DRM)

set (CMAKE_C_COMPILER "arm-linux-gnueabihf-gcc")
set (CMAKE_CXX_COMPILER "arm-linux-gnueabihf-g++")


include_directories(/home/alientek/sdk/rv1126/buildroot/output/alientek_rv1126/build/libdrm-2.4.89)
include_directories(/home/alientek/sdk/rv1126/buildroot/output/alientek_rv1126/build/libdrm-2.4.89/include/drm)

link_directories(/home/alientek/sdk/rv1126/buildroot/output/alientek_rv1126/target/usr/lib)

add_executable(DRM main02.c)

target_link_libraries(DRM libdrm.so)