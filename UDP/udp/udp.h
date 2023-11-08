#pragma once                                                                                       
    
#include <stdio.h>    
#include <stdlib.h>    
#include <unistd.h>    
#include <string.h>    
//网络编程头文件    
#include <sys/types.h>    
#include <sys/stat.h>
#include <sys/socket.h>    
#include <netinet/in.h>    
#include <arpa/inet.h>    
#include <fcntl.h>
#include <iostream>    
#include <string>    
using namespace  std;    
    
class UDP
{    
public:    
        enum FLAG{
                ON,
                OFF
        };
        //构造
        UDP(); 
        //析构
        ~UDP();
        //创建套接字    
        int CreateSocket(const char *addr,uint16_t port);      
   
        //绑定地址
        int Bind(const char *addr,uint16_t port);
        //绑定目的地址,UDP的connect只会检查格式，不会作其他检查
        int Connect();
        //发送字符串数据
        int Sendto(string data);
        //发送数组数据
        int Sendto(char* data,int num);
        //发送数据
        int Write(char* data,int num);
        //接收字符串数据
        int RecvFrom(string *data, struct sockaddr_in* peer_addr=nullptr);     
        //接收数组数据
        int RecvFrom(char *data,int num, struct sockaddr_in* peer_addr = nullptr);
        //接收数据
        int Read(char*data,int num);                              
public:
        bool is_Connect();     
        bool is_Bind();                            
private:
        int sockfd;
        struct sockaddr_in dest_addr;  //目的地址信息
        struct sockaddr_in loacal_addr;//主机地址信息
        bool is_connect = false;
        bool is_bind = false;
};               
