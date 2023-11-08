#include "Object.h"

//初始化静态变量
int Object::m_epollfd = -1;


void Object::modfd(int ev)
{
        epoll_event event;
        event.data.fd = m_sockfd;

        event.events = ev | EPOLLONESHOT | EPOLLRDHUP;
        epoll_ctl(m_epollfd,EPOLL_CTL_MOD,m_sockfd,&event);

}

Object::Object()
{
        m_sockfd = -1;
}

Object::~Object()
{


}

void Object::set_sockfd(int sockfd)
{
        m_sockfd = sockfd;

}
int Object::Read()
{
        int ret = read(m_sockfd,m_read_buf,READ_BUFFER_SIZE);
        if(ret < 0) 
        {
                 printf("%s: read  failed. %s \n", __func__, strerror(errno));
               exit(-1);
        }
        else if(ret == 0)   // tcp 读取长度为0,表示对端关闭连接
        {
                printf("%s: client socket %d disconnet. recv len = 0. \n",__func__, m_sockfd);
                exit(-1);
        }
        else
        {
                printf("%s: read  success.\n", __func__);                
                printf("Read_num:%d\r\n",ret);
                for(int i = 0;i < ret;i++)
                {
                        printf("0x%x ",m_read_buf[i]);
                }
                printf("\r\n");
        }
        strcpy(m_write_buf,"asdfgnini");
        modfd(EPOLLOUT);
        return ret;
}

int Object::Write()
{
        int ret;

       ret =  write(m_sockfd,m_write_buf,strlen(m_write_buf));
        if(ret < 0) 
        {
                 printf("%s: write  failed. %s \n", __func__, strerror(errno));
               exit(-1);
        }
        else
        {
                printf("%s: write  success.\n", __func__);                
                printf("write_num:%d\r\n",ret);
                for(int i = 0;i < ret;i++)
                {
                        printf("0x%x ",m_read_buf[i]);
                }
                printf("\r\n");
        }
        modfd(EPOLLIN);

        return ret;
}