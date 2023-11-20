 #include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


/* 六维力数据结构定义 */
struct PowerData 
{
    float fx;//x轴力
    float fy;//y轴力
    float fz;//z轴力
    float Mx;//x轴力矩
    float My;//y轴力矩
    float Mz;//z轴力矩
};
PowerData LIJU;
int main()
{
        char buf[100] = {0};     
        LIJU.fx = -203.718;
        LIJU.fy = 28.11;
        LIJU.fz = -112.56;
        LIJU.Mx = 406.3;
        LIJU.My = -15.6;
        LIJU.Mz = 45.6;

        sprintf(buf,"LIJU:Fx:%0.2lf,Fy:%0.2lf,Fz:%0.2lf,Mx:%0.2lf,My:%0.2lf,Mz:%0.2lf\r\n"\
        ,LIJU.fx,LIJU.fy,LIJU.fz,LIJU.Mx,LIJU.My,LIJU.Mz);
        int fd = open("./udp.tmp",O_WRONLY);
        if (fd < 0)
        {
                printf("[TEST]  open fail\r\n");
        }
        while (1)
        {
                write(fd,buf,strlen(buf));
                sleep(1);
        }
        
        return 0;
}


