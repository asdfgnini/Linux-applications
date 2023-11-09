/*
 * 作者：zjt
 * */
#define _GNU_SOURCE     //在源文件开头定义_GNU_SOURCE宏
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <string.h>
#include <termios.h>
#include <signal.h>


/**************************************串口配置变量******************************/
typedef struct uart_hardware_cfg {
    unsigned int baudrate;      /* 波特率 */
    unsigned char dbit;         /* 数据位 */
    char parity;                /* 奇偶校验 */
    unsigned char sbit;         /* 停止位 */
} uart_cfg_t;

static struct termios old_cfg;  //用于保存终端的配置参数
static int fd;      //串口终端对应的文件描述符
// char *device = "/dev/ttyUSB0";

//funtion api
static int uart_init(const char *device);
static int uart_cfg(const uart_cfg_t *cfg);
static void async_io_init();
static void io_handler(int sig, siginfo_t *info, void *context);


/***************************调试信息控制*********************************/
// #define __Debug_info

/**************************速度*****************************************/
#define GPS_Buffer_Length 60
typedef struct SaveData
{
    char GPS_Buffer[GPS_Buffer_Length];
    struct 
    {
       int16_t x;
       int16_t y;
       int16_t z;
    }raw;
    float x;
    float y;
    float z;
    int isGet;
} _SaveData;
_SaveData Save_Data;


int pos = 0;
/**************************************************************************************************************************/
/******************************************************主程序***************************************************************/
/**************************************************************************************************************************/

int main(int argc,char* argv[])
{
    //设定程序输出为及时处理，Clion默认程序输出为暂存：即setbuf(stdout,outbuf);
    setbuf(stdout,NULL);//puts到输出台上

    uart_cfg_t cfg = {0};

    //串口初始
    uart_init(argv[1]);
    //设置串口参数 
    uart_cfg(&cfg);
    //设置IO策略为异步IO
    async_io_init();

    for(;;)
    {
        
        /*****伪代码******/
        if (flag)
        {
            flag = 0;//设立一个标志位，在信号处理函数中置1,在此处清0,以实现同步

            //添加自己的逻辑代码
        }
        
        sleep(1);//根据实际情况选择是否添加延时,有些时候不添加更好，有些时候添加更好

    }

    /* 退出 */
    tcsetattr(fd, TCSANOW, &old_cfg);   //恢复到之前的配置
    close(fd);
    exit(EXIT_SUCCESS);

    return 0;
}


/**
 ** 串口初始化操作
 ** 参数device表示串口终端的设备节点
 **/
static int uart_init(const char *device)
{
    /* 打开串口终端  使用的标志有 可读可写，告诉系统该节点不会成为进程的控制终端，非阻塞方式，读不到数据返回-1,*/
    fd = open(device, O_RDWR | O_NOCTTY | O_NONBLOCK);
    if (0 > fd) {
        fprintf(stderr, "open error: %s: %s\n", device, strerror(errno));
        return -1;
    }

    /* 获取串口当前的配置参数 */
    if (0 > tcgetattr(fd, &old_cfg)) {
        fprintf(stderr, "tcgetattr error: %s\n", strerror(errno));
        close(fd);
        return -1;
    }

    return 0;
}

/**
 ** 串口配置
 ** 参数cfg指向一个uart_cfg_t结构体对象
 **/
static int uart_cfg(const uart_cfg_t *cfg)
{
    struct termios new_cfg = {0};   //将new_cfg对象清零
    speed_t speed;

    /* 设置为原始模式 */
    cfmakeraw(&new_cfg);

    /* 使能接收 */
    new_cfg.c_cflag |= CREAD;

    /* 设置波特率 */
    switch (cfg->baudrate) {
        case 1200: speed = B1200;
            break;
        case 1800: speed = B1800;
            break;
        case 2400: speed = B2400;
            break;
        case 4800: speed = B4800;
            break;
        case 9600: speed = B9600;
            break;
        case 19200: speed = B19200;
            break;
        case 38400: speed = B38400;
            break;
        case 57600: speed = B57600;
            break;
        case 115200: speed = B115200;
            break;
        case 230400: speed = B230400;
            break;
        case 460800: speed = B460800;
            break;
        case 500000: speed = B500000;
            break;
        default:    //默认配置为115200
            speed = B9600;
            printf("default baud rate: 9600\n");
            break;
    }

    if (0 > cfsetspeed(&new_cfg, speed)) {
        fprintf(stderr, "cfsetspeed error: %s\n", strerror(errno));
        return -1;
    }

    /* 设置数据位大小 */
    new_cfg.c_cflag &= ~CSIZE;  //将数据位相关的比特位清零
    switch (cfg->dbit) {
        case 5:
            new_cfg.c_cflag |= CS5;
            break;
        case 6:
            new_cfg.c_cflag |= CS6;
            break;
        case 7:
            new_cfg.c_cflag |= CS7;
            break;
        case 8:
            new_cfg.c_cflag |= CS8;
            break;
        default:    //默认数据位大小为8
            new_cfg.c_cflag |= CS8;
            printf("default data bit size: 8\n");
            break;
    }

    /* 设置奇偶校验 */
    switch (cfg->parity) {
        case 'N':       //无校验
            new_cfg.c_cflag &= ~PARENB;
            new_cfg.c_iflag &= ~INPCK;
            break;
        case 'O':       //奇校验
            new_cfg.c_cflag |= (PARODD | PARENB);
            new_cfg.c_iflag |= INPCK;
            break;
        case 'E':       //偶校验
            new_cfg.c_cflag |= PARENB;
            new_cfg.c_cflag &= ~PARODD; /* 清除PARODD标志，配置为偶校验 */
            new_cfg.c_iflag |= INPCK;
            break;
        default:    //默认配置为无校验
            new_cfg.c_cflag &= ~PARENB;
            new_cfg.c_iflag &= ~INPCK;
            printf("default parity: N\n");
            break;
    }

    /* 设置停止位 */
    switch (cfg->sbit) {
        case 1:     //1个停止位
            new_cfg.c_cflag &= ~CSTOPB;
            break;
        case 2:     //2个停止位
            new_cfg.c_cflag |= CSTOPB;
            break;
        default:    //默认配置为1个停止位
            new_cfg.c_cflag &= ~CSTOPB;
            printf("default stop bit size: 1\n");
            break;
    }

    /* 将MIN和TIME设置为0 */
    new_cfg.c_cc[VTIME] = 0;
    new_cfg.c_cc[VMIN] = 0;

    /* 清空缓冲区 */
    if (0 > tcflush(fd, TCIOFLUSH)) {
        fprintf(stderr, "tcflush error: %s\n", strerror(errno));
        return -1;
    }

    /* 写入配置、使配置生效 */
    if (0 > tcsetattr(fd, TCSANOW, &new_cfg)) {
        fprintf(stderr, "tcsetattr error: %s\n", strerror(errno));
        return -1;
    }

    /* 配置OK 退出 */
    return 0;
}

/**
 ** 异步I/O初始化函数
 **/
static void async_io_init()
{
    struct sigaction sigatn;
    int flag;

    /* 使能异步I/O */
    flag = fcntl(fd, F_GETFL);  //使能串口的异步I/O功能
    flag |= O_ASYNC;
    fcntl(fd, F_SETFL, flag);

    /* 设置异步I/O的所有者 */
    fcntl(fd, F_SETOWN, getpid());

    /* 指定实时信号SIGRTMIN作为异步I/O通知信号 */
    fcntl(fd, F_SETSIG, SIGRTMIN);

    /* 为实时信号SIGRTMIN注册信号处理函数 */
    sigatn.sa_sigaction = io_handler;   //当串口有数据可读时，会跳转到io_handler函数
    sigatn.sa_flags = SA_SIGINFO;
    sigemptyset(&sigatn.sa_mask);
    sigaction(SIGRTMIN, &sigatn, NULL);
}

/**
 ** 信号处理函数，当串口有数据可读时，会跳转到该函数执行
 **/
static void io_handler(int sig, siginfo_t *info, void *context)
{    

    if(SIGRTMIN != sig)
        return;

    /* 判断串口是否有数据可读 */
    if (POLL_IN == info->si_code)
    {
          /***********伪代码**********/
          flag = 1;//在此处将标志位置1,以实现同步
          //在此处读取数据                

    }     
}
