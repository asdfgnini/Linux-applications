#include "../udp/udp.h"

UDP    udp;
int main(int argc,char*argv[])
{
        //创建套接字，参数传入目的IP和端口
        udp.CreateSocket("192.168.10.50",8080);
        //创建数据
        char sendData[5];           
        sendData[0] = 0x01;
        sendData[1] = 0x03;
        sendData[2] = 0x02;
        sendData[3] = 0x00;
        sendData[4] = 0x12;
        //接收缓冲区
        char buffer[1024] = {0};
        //先绑定，后连接
        udp.Bind("192.168.10.100",8080);            
        udp.Connect();           
        while(1)
        {       
                udp.Write(sendData,sizeof(sendData));    
                udp.Read(buffer,sizeof(buffer));
                sleep(1);
        }
        return 0;
}






