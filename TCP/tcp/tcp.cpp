#include "tcp.h"


TCP::TCP()
{
        sockfd = -1;
}

TCP::~TCP()
{
        close(sockfd);
}

 //创建套接字    
int TCP::CreateSocket(const char *addr,uint16_t port)
{
        sockfd = socket(PF_INET,SOCK_STREAM,0);
        if(sockfd < 0)
        {
                perror("socket create fail");
                return -1;
        }
        dest_addr.sin_family = AF_INET;    
        dest_addr.sin_port = htons(port);    
        dest_addr.sin_addr.s_addr = inet_addr(addr);  

        is_client = true;
        return 0;
}

 //创建套接字    
int TCP::CreateSocket()
{
        sockfd = socket(PF_INET,SOCK_STREAM,0);
        if(sockfd < 0)
        {
                perror("socket create fail");
                return -1;
        }
        is_server = true;
        return 0;
}
//绑定地址
int TCP::Bind(const char *addr,uint16_t port)
{
        //填充数据结构    
        loacal_addr.sin_family = AF_INET;
        loacal_addr.sin_port = htons(port);
        loacal_addr.sin_addr.s_addr = inet_addr(addr);
        int ret = bind(sockfd,(struct sockaddr*)&loacal_addr,sizeof(loacal_addr));
        if(ret < 0)  
        {
                perror("bind fail");
                return -1;
        }
        else
        {
                printf("%s:bind %s:%d success\n",__func__,addr,port);
        }

        return 0;
}
//绑定目的地址,
int TCP::Connect()
{
        int dest_addr_len = sizeof(dest_addr);
       int ret =  connect(sockfd,(struct sockaddr*)&dest_addr,dest_addr_len);
        char *ip = inet_ntoa(dest_addr.sin_addr); 
        inet_ntop(dest_addr.sin_family, &dest_addr.sin_addr, ip, dest_addr_len);
        int port = ntohs(dest_addr.sin_port);
        if(ret < 0) 
        {
                printf("%s: connect failed. %s \n", __func__, strerror(errno));
                return 0;
        }
        else
        {
                printf("%s: connect %s:%d success.\n", __func__, ip, port);
              
        }
        return true;

}
//监听
int TCP:: Listen()
{
        int ret;
        ret = listen(sockfd,5);
        if(ret == -1)
        {
                printf("%s: listen failed. %s\n", __func__, strerror(errno));
                return 1;
        }
        else
        {
                printf("%s: listening ...\n", __func__);
        }
        return ret;
}

//接受
int TCP::Accept(struct sockaddr_in* peer_addr)
{
        socklen_t peer_addr_len;
        if(peer_addr != nullptr)
        {
                peer_addr_len = sizeof(struct sockaddr_in);
        }
        else
        {
                peer_addr_len = 0;
        }
        acceptfd = accept(sockfd,(struct sockaddr*)peer_addr,&peer_addr_len);
        if(acceptfd < 0)
         {
                printf("%s: accept error. %s\n", __func__, strerror(errno));
                return 1;
        }
        else
        {
                if(peer_addr != nullptr)
                {
                        char *ip = inet_ntoa(peer_addr->sin_addr); 
                        inet_ntop(peer_addr->sin_family, &peer_addr->sin_addr, ip, peer_addr_len);
                        int port = ntohs(peer_addr->sin_port);
                        printf("%s: client [%s:%d] \n", __func__, ip, port);    
                }
        }
        return acceptfd;
}
//发送数据
int TCP:: Write(char* data,int num)
{
        int ret;
        if(is_client)
        {
                ret = write(sockfd,data,num);
        }
        else if(is_server)
        {
                ret = write(acceptfd,data,num);
        }
        if(ret < 0) 
        {
                printf("%s: write  failed. %s \n", __func__, strerror(errno));
                return 0;
        }
        else
        {
                printf("%s: write  success.\n", __func__);
                printf("Send_num:%d\r\n",ret);
                for(int i = 0;i < ret;i++)
                {
                        printf("0x%x ",data[i]);
                }
                printf("\r\n");
        }
        return ret;
}

//接收数据
int TCP::Read(char *data ,int num)
{
        int ret;
        if(is_client)
        {
                ret = read(sockfd,data,num); 
        }
        else if(is_server)
        {
                ret = read(acceptfd,data,num);
        }
        if(ret < 0) 
        {
                printf("%s: read  failed. %s \n", __func__, strerror(errno));
               exit(-1);
        }
        else if(ret == 0)   // tcp 读取长度为0,表示对端关闭连接
        {
                printf("%s: client socket %d disconnet. recv len = 0. \n",__func__, sockfd);
                exit(-1);
        }
        else
        {
                printf("%s: read  success.\n", __func__);                
                printf("Read_num:%d\r\n",ret);
                for(int i = 0;i < ret;i++)
                {
                        printf("0x%x ",data[i]);
                }
                printf("\r\n");
        }

        return ret;
}

