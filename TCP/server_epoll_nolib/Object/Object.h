#pragma once

#include "unistd.h"
#include "stdio.h"
#include <stdlib.h>    
#include <string.h>    
#include <errno.h>
#include <sys/epoll.h>


class Object
{
public:
        static const int READ_BUFFER_SIZE = 2048;
        static const int WRITE_BUFFER_SIZE = 1024;
        static int m_epollfd;
private:
        int m_sockfd;
        char m_read_buf[READ_BUFFER_SIZE];
        char m_write_buf[WRITE_BUFFER_SIZE];
public:
        Object();
        ~Object();
        void set_sockfd(int sockfd);

public:
        int Read();
        int Write();
        void modfd(int ev);
};

