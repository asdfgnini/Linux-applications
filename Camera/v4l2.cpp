#include "v4l2.h"

v4l2::v4l2(string devpath)
{
    this->devpath = devpath;
}

v4l2::~v4l2()
{
    if(fd != -1)
    {
        close(fd);
    }
}

bool v4l2::Open()
{
    fd = open(devpath.c_str(),O_RDWR);
    if(fd < 0)
    {
        perror("打开设备失败");
        exit(-1);
    }
    return true;

}

void v4l2::Get_device_info()
{


}