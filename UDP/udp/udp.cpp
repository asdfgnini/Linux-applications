#include "udp.h"


UDP::UDP()    
{    
        sockfd = -1;    

}  

//创建套接字    
int UDP::CreateSocket( const char *addr,uint16_t port)    
{    
        sockfd = socket(AF_INET, SOCK_DGRAM, 0);    
        if(sockfd < 0)    
        {    
                perror("socket create fail");    
                return -1;    
        }    
        dest_addr.sin_family = AF_INET;    
        dest_addr.sin_port = htons(port);    
        dest_addr.sin_addr.s_addr = inet_addr(addr);  


        return 0;    
}

UDP::~UDP()
{
          close(sockfd);
}

//绑定地址
int UDP::Bind(const char *addr,uint16_t port)
{
        //填充数据结构    
         bzero(&loacal_addr, sizeof(loacal_addr));           //将整个结构体清零
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
        is_bind = true;
        return 0;
}
//绑定目的地址
int UDP::Connect()
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
                is_connect = true;
        }
        return true;
}

//发送字符串数据
int UDP::Sendto(string data)
{
        int dest_addr_len = sizeof(dest_addr);
        ssize_t send_size = sendto(sockfd, data.c_str(),data.size(),0,(struct sockaddr*)&dest_addr, dest_addr_len);
        if(send_size < 0)
        {
                perror("sendto");
                return -1;
        }
        else
        {

                printf("%s: write  success.\n", __func__);
                printf("Send_num:%ld\r\n",send_size);
                cout << data <<endl;
        }
        return send_size;
}
//发送数组数据
int UDP::Sendto(char* data,int num)
{
        int dest_addr_len = sizeof(dest_addr);
        ssize_t send_size = sendto(sockfd, data,num,0,(struct sockaddr*)&dest_addr, dest_addr_len);
        if(send_size < 0)
        {
                perror("sendto fail");
                return -1;
        }
        else
        {
                //  printf("%s: write  success.\n", __func__);
                // printf("Send_num:%ld\r\n",send_size);
                // for(int i = 0;i < send_size;i++)
                // {
                //         printf("0x%x ",data[i]);
                // }
                // printf("\r\n");                 
        }

        return send_size;
}
int UDP::Write(char* data,int num)
{
         int ret;
        ret =is_Connect(); 
        if(ret != true)
        {
                printf("Please connect First\r\n");     
                exit(-1);   
        }
       ret =  write(sockfd,data,num);
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
//接收字符串数据
int UDP::RecvFrom(string *data, struct sockaddr_in* peer_addr)                                      
{
        char buf[1024] = {0};
        socklen_t peer_addr_len;
        if(peer_addr != nullptr)
        {
                peer_addr_len = sizeof(struct sockaddr_in);
        }
        else
        {
                peer_addr_len = 0;
        }
        ssize_t recv_size = recvfrom(sockfd,buf,sizeof(buf) , 0,(struct sockaddr*)peer_addr,&peer_addr_len);

        if(recv_size < 0)
        {
                perror("recvfrom");
                return -1;
        }
        else
        {
                printf("%s: read  success.\n", __func__);       
                data->assign(buf,strlen(buf));

                if(peer_addr != nullptr)
                {
                        char *ip = inet_ntoa(peer_addr->sin_addr); 
                        inet_ntop(peer_addr->sin_family, &peer_addr->sin_addr, ip, peer_addr_len);
                        int port = ntohs(peer_addr->sin_port);
                        printf("%s: client [%s:%d] \n", __func__, ip, port);    
                }
                printf("recv_num:%ld\r\n",recv_size);   
                cout << data <<endl;               
        }

        return recv_size;
}
//接收数组数据
int UDP::RecvFrom(char *data,int num, struct sockaddr_in* peer_addr)                                      
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

        ssize_t recv_size = recvfrom(sockfd,data,num , 0,(struct sockaddr*)peer_addr,&peer_addr_len);
        if(recv_size < 0)
        {
                perror("recvfrom fail");
                return -1;
        }
        else
        {
                printf("%s: read  success.\n", __func__);       
                if(peer_addr != nullptr)
                {
                        char *ip = inet_ntoa(peer_addr->sin_addr); 
                        inet_ntop(peer_addr->sin_family, &peer_addr->sin_addr, ip, peer_addr_len);
                        int port = ntohs(peer_addr->sin_port);
                        printf("%s: client [%s:%d] \n", __func__, ip, port);    
                }
                printf("recv_num:%ld\r\n",recv_size);     
                for(int i = 0;i < recv_size;i++)
                {
                        printf("0x%x ",data[i]);
                }
                printf("\r\n");                
        }


        return recv_size;
}
int UDP::Read(char*data,int num)
{
        int ret;
       ret = is_Bind();
       if(ret != true)
       {
               printf("Please Bind first\r\n");
               exit(-1);
       }
        ret =  read(sockfd,data,num);    
        if(ret < 0) 
        {
                printf("%s: read  failed. %s \n", __func__, strerror(errno));
                return 0;
        }
        else
        {
                printf("%s: read  success.\n", __func__);                
                socklen_t peer_addr_len = sizeof(dest_addr);  
                char *ip = inet_ntoa(dest_addr.sin_addr); 
                inet_ntop(dest_addr.sin_family, &dest_addr.sin_addr, ip, peer_addr_len);
                int port = ntohs(dest_addr.sin_port);
                printf("%s: client [%s:%d] \n", __func__, ip, port);    


                printf("Read_num:%d\r\n",ret);
                for(int i = 0;i < ret;i++)
                {
                        printf("0x%x ",data[i]);
                }
                printf("\r\n");
        }

        return ret;
}


bool UDP::is_Connect()
{
        return is_connect;
}
bool UDP::is_Bind()
{
        return is_bind;
}

