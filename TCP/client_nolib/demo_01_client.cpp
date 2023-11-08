
#include "../tcp/tcp.h"

TCP tcp;
int main()
{
        //输入目的地址
        tcp.CreateSocket("192.168.10.50",8080);
        tcp.Connect();

        //创建数据
        char sendData[5];           
        sendData[0] = 0x01;
        sendData[1] = 0x03;
        sendData[2] = 0x02;
        sendData[3] = 0x00;
        sendData[4] = 0x12;
        char buffer[5] = {0};
        while (1)
        {
                tcp.Write(sendData,sizeof(sendData));
                tcp.Read(buffer,sizeof(buffer));
                sleep(1);

        }

        return 0;
}

