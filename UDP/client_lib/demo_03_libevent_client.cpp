#include  "event2/event.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>

void on_read(evutil_socket_t sock, short events, void *arg)
{
        char buffer[1024];
        int len = recvfrom(sock, buffer, sizeof(buffer), 0,nullptr,0);
        if (len > 0)
        {
                buffer[len] = '\0';
                printf("Received: %s\n", buffer);
        }
}

void time_cb(evutil_socket_t fd, short events, void *arg)
{
        printf("123\r\n");
        
}


int main(int argc, char **argv)
{
        // 初始化Libevent
        struct event_base *base = event_base_new();
        if (!base)
        {
                fprintf(stderr, "Failed to initialize event base.\n");
                return 1;
        }

        // 创建UDP套接字
        evutil_socket_t sock = socket(AF_INET, SOCK_DGRAM, 0);
        if (sock < 0)
        {
                fprintf(stderr, "Failed to create socket.\n");
                return 1;
        }
        // 绑定套接字
        struct sockaddr_in addr;
        memset(&addr, 0, sizeof(addr));
        addr.sin_family = AF_INET;
        addr.sin_port = htons(8080);
        addr.sin_addr.s_addr = htonl(INADDR_ANY);
        if (bind(sock, (struct sockaddr*)&addr, sizeof(addr)) < 0) 
        {
                fprintf(stderr, "Failed to bind socket.\n");
                return 1;
        }

        // 设置套接字为非阻塞
        evutil_make_socket_nonblocking(sock);

        // 创建事件
        struct event *ev = event_new(base, sock, EV_READ|EV_PERSIST, on_read, NULL);
        if (!ev)
        {
                fprintf(stderr, "Failed to create event.\n");
                return 1;
        }
        struct event *ev1 = event_new(base,-1,EV_PERSIST,time_cb,NULL);
        if (!ev1)
        {
                fprintf(stderr, "Failed to create event.1\n");
                return 1;
        }
        // 添加事件
        if (event_add(ev, NULL) < 0)
        {
                fprintf(stderr, "Failed to add event.\n");
                return 1;
        }
        struct timeval tv;
        tv.tv_sec = 2;
        tv.tv_usec = 0;
        // 添加事件
        if (event_add(ev1, &tv) < 0)
        {
                fprintf(stderr, "Failed to add event.1\n");
                return 1;
        }

    // 进入事件循环
    event_base_dispatch(base);
}