#include "iostream"
#include <iostream>
#include <sys/epoll.h>
 #include <sys/types.h>          
#include <sys/socket.h>
#include <sys/types.h>    
#include <sys/socket.h>    
#include <netinet/in.h>    
#include <arpa/inet.h>   
#include <fcntl.h>
#include <error.h>

#include  "event2/event.h"

static int listen_fd;

int main()
{
        //创建套接字
        listen_fd = socket(PF_INET,SOCK_STREAM,0);
        if(listen_fd < 0)
        {
                printf("%s : socket fail\r\n",__func__);
        }
        //绑定ip地址信息
        struct sockaddr_in local_addr;
        local_addr.sin_family = AF_INET;
        local_addr.sin_port = htons(8080);
        local_addr.sin_addr.s_addr = inet_addr("192.168.10.50");
        int ret = bind(listen_fd,(sockaddr*)&local_addr,sizeof(local_addr));
        if (ret < 0)
        {
                printf("%s : bind fail\r\n",__func__);
        }
        //监听套接字
        ret = listen(listen_fd,5);
        if (ret < 0)
        {
                printf("%s : listen fail\r\n",__func__);
        }        
        // 初始化Libevent
        struct event_base *base = event_base_new();
        if (!base)
        {
                fprintf(stderr, "Failed to initialize event base.\n");
                return 1;
        }




        return 0;
}