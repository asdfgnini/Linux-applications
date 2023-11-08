
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

#include "./Object/Object.h"



#define MAX_CLIENT_FD 65536
#define MAX_EPOLL_FD 10000

static int listen_fd;
static int epoll_fd;
static Object connect_fd[MAX_CLIENT_FD];


//对文件描述符设置非阻塞
int setnonblocking(int fd)
{
        int old_option = fcntl(fd, F_GETFL);
        int new_option = old_option | O_NONBLOCK;
        fcntl(fd, F_SETFL, new_option);
        return old_option;
}
void addfd(int epollfd, int fd, bool one_shot)
{
        epoll_event event;
        event.data.fd = fd;

        event.events = EPOLLIN | EPOLLRDHUP;
        if(one_shot)
        {
                 event.events |= EPOLLONESHOT;
        }
        epoll_ctl(epollfd,EPOLL_CTL_ADD,fd,&event);
        setnonblocking(fd);
}


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
        //创建epoll套接字，即创建监控池，与libevent中的event_base很相似
        epoll_fd = epoll_create(5);
        if (epoll_fd < 0)
        {
                printf("%s : epoll_create fail\r\n",__func__);
        }
        Object::m_epollfd = epoll_fd;
        //将listen_fd绑定事件并加入监控池,同时设置listen_fd为非阻塞
        addfd(epoll_fd,listen_fd,false);
        //创建事件返回数组
        epoll_event events[MAX_EPOLL_FD];
       
       while(1)
       {
               int number = epoll_wait(epoll_fd,events,MAX_EPOLL_FD,-1);
                if (number < 0 && errno != EINTR)
                {
                        printf("%s : epoll_wait fail\r\n",__func__);
                        break;
                }
                for (int i = 0; i < number; i++)
                {
                        int sockfd = events[i].data.fd;                     
                        if(sockfd == listen_fd)
                        {
                                struct sockaddr_in client_addr;
                                socklen_t client_addr_len = sizeof(client_addr);                      
                                int conect_fd =  accept(sockfd,(sockaddr*)&client_addr,&client_addr_len);
                                if (conect_fd < 0)
                                {
                                        printf("%s:errno is:%d", "accept error", errno);
                                        continue;
                                }
                                //打印客户端信息
                                char *ip = inet_ntoa(client_addr.sin_addr); 
                                inet_ntop(client_addr.sin_family, &client_addr.sin_addr, ip, client_addr_len);
                                int port = ntohs(client_addr.sin_port);
                                printf("%s: client [%s:%d] \n", __func__, ip, port);    
                                //将新客户端添加到客户端池中
                                connect_fd[conect_fd].set_sockfd(conect_fd); 
                                //将新客户端加入监控池
                                addfd(epoll_fd,conect_fd,false);                        
                        }
                        else if(events[i].events & EPOLLIN)
                        {
                                printf("EPOLLIN event\r\n");
                                connect_fd[sockfd].Read();
                        }
                        else if(events[i].events & EPOLLOUT)
                        {
                                printf("EPOLLOUT event\r\n");
                                connect_fd[sockfd].Write(); 
                        }
                }     
       }

        return 0;
}
