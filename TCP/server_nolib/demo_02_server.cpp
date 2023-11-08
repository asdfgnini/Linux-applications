
#include "../tcp/tcp.h"

TCP tcp;
int main()
{
        //输入目的地址
        tcp.CreateSocket();
        //绑定
        tcp.Bind("192.168.10.50",8080);

        //监听
        tcp.Listen();
        //接收
        struct sockaddr_in peer_addr;      
        tcp.Accept(&peer_addr);          
        //接收缓冲区
        char buffer[1024] = {0};

        while (1)
        {
               int ret = tcp.Read(buffer,sizeof(buffer));
                tcp.Write(buffer,ret);
        }

        return 0;
}

