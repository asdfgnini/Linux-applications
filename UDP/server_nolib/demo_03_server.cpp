#include "../udp/udp.h"
#include <sys/epoll.h>


// #define __Debug_info


UDP     udp;
int fd = -1;//管道描述符
static int epoll_fd;//红黑树
#define MAX_EPOLL_FD 10000
char buffer[4086] = {0};


//修改epoll策略
void modfd(int epollfd,int fd,int ev)
{
        epoll_event event;
        event.data.fd = fd;

        event.events = ev | EPOLLONESHOT | EPOLLRDHUP;
        epoll_ctl(epollfd,EPOLL_CTL_MOD,fd,&event);

}
//对文件描述符设置非阻塞
int setnonblocking(int fd)
{
        int old_option = fcntl(fd, F_GETFL);
        int new_option = old_option | O_NONBLOCK;
        fcntl(fd, F_SETFL, new_option);
        return old_option;
}
//添加文件描述符到红黑树
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
        //创建UDP套接字，创建目标地址信息
        udp.CreateSocket("192.168.10.200",8080);
        //判断管道是否存在
        const char *fifo_name = "./udp.tmp";
        if (access(fifo_name, F_OK) == -1)
        {
                //创建管道
                int ret = mkfifo("udp.tmp",0777);
                if(ret == -1)
                {
                        perror("[UDP_S]  mkfifo");                        
                }                
        }

         fd = open("udp.tmp",O_RDONLY);
        if (fd == -1)
        {
                perror("[UDP_S]  open");
                exit(1);
        }
        //创建epoll套接字，即创建监控池，与libevent中的event_base很相似
        epoll_fd = epoll_create(5);
        if (epoll_fd < 0)
        {
                printf("[UDP_S]  %s : epoll_create fail\r\n",__func__);
        }
        //将listen_fd绑定事件并加入监控池,同时设置listen_fd为非阻塞
        addfd(epoll_fd,fd,false);
        //创建事件返回数组
        epoll_event events[MAX_EPOLL_FD];
        while(1)
        {

                 int number = epoll_wait(epoll_fd,events,MAX_EPOLL_FD,-1);
                if (number < 0 && errno != EINTR)
                {
                        printf("[UDP_S]  %s : epoll_wait fail\r\n",__func__);
                        break;
                }
                 for (int i = 0; i < number; i++)
                {
                        int sockfd = events[i].data.fd;   
                        //可读            
                        if(events[i].events & EPOLLIN)
                        {

                                int re_value =  read(fd,buffer,sizeof(buffer));
                                if(re_value < 0) 
                                {
                                        printf("[UDP_S]  %s: read  failed. %s \n", __func__, strerror(errno));
                                        exit(-1);
                                }
                                else
                                {

#ifdef __Debug_info
                                        printf("[UDP_S]  %s: read  success.\n", __func__);                
                                        printf("[UDP_S]  Read_num:%d\r\n",re_value);
                                        for(int i = 0;i < re_value;i++)
                                        {
                                                printf("0x%x ",buffer[i]);
                                        }
                                        printf("\r\n");
#endif
                                        //发送数据
                                         udp.Sendto(buffer,strlen(buffer));
                                         bzero(buffer,sizeof(buffer));
                                        modfd(epoll_fd,sockfd,EPOLLIN);
                                }
                        }    
                        else if (events[i].events & (EPOLLRDHUP | EPOLLHUP | EPOLLERR))     //异常处理
                        {
                                //监测其他进程是否在线
                                epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, 0);
                                close(fd);
                                unlink("udp.tmp");

                                //判断管道是否存在
                                const char *fifo_name = "./udp.tmp";
                                if (access(fifo_name, F_OK) == -1)
                                {
                                        //创建管道
                                        int ret = mkfifo("udp.tmp",0777);
                                        if(ret == -1)
                                        {
                                                perror("[UDP_S]  mkfifo");                        
                                        }           
                                }
#ifdef __Debug_info
                                printf("OK123\r\n");
#endif
                                fd = open("udp.tmp",O_RDONLY);
                                if (fd == -1)
                                {
                                        perror("[UDP_S]  open");
                                        exit(1);
                                }
                                addfd(epoll_fd,fd,false);
                        }           
                }
        }
        return 0;
}





//未使用
#if 0    
int detect_process(const char *process_name) 
{
        FILE *ptr;
        char buff[512];
        char ps[128];
        memset(buff, 0, sizeof(buff));
        memset(ps, 0, sizeof(ps));
        sprintf(ps, "ps -w | grep -v grep | grep %s", process_name);
        if ((ptr = popen(ps, "r")) != NULL) 
        {
                while (fgets(buff, sizeof(buff), ptr) != NULL) 
                {
                        if (strstr(buff, process_name) != NULL)
                        {
                                pclose(ptr);
                                return 1;
                        }
                }
                pclose(ptr);
        }
        return 0;
}

void reload_init()
{
        if (detect_ret == 0 && detect_ret01 == 0)
        {
                epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, 0);
                close(fd);
                unlink("udp.tmp");
                
                //判断管道是否存在
                const char *fifo_name = "./udp.tmp";
                if (access(fifo_name, F_OK) == -1)
                {
                        //创建管道
                        int ret = mkfifo("udp.tmp",0777);
                        if(ret == -1)
                        {
                                perror("[UDP_S]  mkfifo");                        
                        }           
                }
                printf("OK123\r\n");
                fd = open("udp.tmp",O_RDONLY);
                if (fd == -1)
                {
                        perror("[UDP_S]  open");
                        exit(1);
                }
                addfd(epoll_fd,fd,false);
        }
}



        char cmd[256];
	memset(cmd, 0, 256);
        char pid[6] = {0};
        int pid_num = 0;
        //判断UDP服务器进程是否存在
       int ret =  detect_process("UDP_S");
        if(ret)
        {
                printf("OK\r\n");
                FILE *fp;
                fp = fopen("out.txt", "w");
                fclose(fp);

                sprintf(cmd, "ps -e | grep %s | awk '{print $1}'   > ./out.txt", "UDP_S");
                system(cmd);
                int fd01 = open("./out.txt",O_RDONLY);
                if(fd01 < 0)
                {
                        printf("fail\r\n");
                }
                read(fd01,pid,sizeof(pid));
                close(fd01);
                remove("out.txt");        
                printf("pid = %s\r\n",pid);
               pid_num =  atoi(pid);
                sprintf(cmd, "kill %d", pid_num);
                system(cmd);
        }
         //判断管道是否存在
        const char *fifo_name = "./udp.tmp";
        if (access(fifo_name, F_OK) == 0)
        {
                remove("udp.tmp");
        }

#endif