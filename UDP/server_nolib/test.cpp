 #include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>



typedef double _Float64;
/* GPS数据结构定义 */
struct GpsData
{
   uint64_t timestamp; //UTC时间
   _Float64  latitude;   //纬度
   _Float64  longitude;  //经度
   _Float64  altitude;   //海拔
    bool   sentenceHasFix; // 定位是否有效
    size_t satellites;  //卫星数量
   _Float64 hdop;       //精度因子
   _Float64 speed;      //水平速率
   _Float64 course;     //地面航向
};
GpsData GPS;

int main()
{
         char buf[100] = {0};     
        GPS.timestamp = 130852.718;
        GPS.latitude = 28.11;
        GPS.longitude = 112.56;
        GPS.altitude = 45.6;

        sprintf(buf,"GPS:%ld,weidu:%0.2lf,jingdu:%0.2lf,haiba:%0.2lf\r\n",GPS.timestamp,GPS.latitude,GPS.longitude,GPS.altitude);
        int fd = open("./udp.tmp",O_WRONLY);
        if (fd < 0)
        {
                printf("[TEST]  open fail\r\n");
        }

        while (1)
        {
                write(fd,buf,sizeof(buf));

                sleep(1);
        }
        
        return 0;
}


