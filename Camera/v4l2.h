#pragma once

#include <cstdio>
#include <cstdlib>
#include <string>
using namespace std;

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

class v4l2
{
private:
    int fd;
    string devpath;
    




public:
    v4l2(string devpath);
    ~v4l2();


public:
    bool Open();//打开设备
    void Get_device_info();//获取设备信息
    



};

