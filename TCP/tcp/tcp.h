#pragma once

#include <stdio.h>    
#include <stdlib.h>    
#include <unistd.h>    
#include <string.h>    
//网络编程头文件    
#include <sys/types.h>    
#include <sys/socket.h>    
#include <netinet/in.h>    
#include <arpa/inet.h>    
    
#include <iostream>    
#include <string>    
using namespace  std;    
class TCP
{
public:
        //构造
        TCP();
        //析构
        ~TCP();
        //创建套接字    
        int CreateSocket(const char *addr,uint16_t port);      
        //创建套接字    
        int CreateSocket();    
        //绑定地址
        int Bind(const char *addr,uint16_t port);  
        //绑定目的地址,
        int Connect();
        //监听
        int Listen();
        //接受
        int Accept(struct sockaddr_in* peer_addr=nullptr);
        //发送数据
        int Write(char* data,int num);
        //接收数据
        int Read(char *data ,int num);
private:
        int sockfd;//监听套接字
        int acceptfd;//连接套接字
        bool is_client = false;
        bool is_server = false;
        struct sockaddr_in dest_addr;
        struct sockaddr_in loacal_addr;




};