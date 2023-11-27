#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <termios.h>
#include <stdbool.h>
#include <stdlib.h>



// #define __Debug_info


//获取原始数据
unsigned char Get_data_raw[8] = {0x01,0x03,0x00,0xC8,0x00,0x0C,0xC4,0x31};
//获取解耦数据
unsigned char Get_data[8] = {0x01,0x03,0x00,0xD4,0x00,0x0C,0x05,0xF7};
//校准
unsigned char Jiaozhun[13] = {0x01,0x10,0x00,0x00,0x00,0x02,0x04,0x00,0x00,0x27,0x10,0xE9,0x93};

#define LIJU_Buffer_Length 30
typedef struct SaveData
{
    unsigned long fx;//x轴力
    unsigned long fy;//y轴力
    unsigned long fz;//z轴力
    unsigned long Mx;//x轴力矩
    unsigned long My;//y轴力矩
    unsigned long Mz;//z轴力矩
	unsigned long LIJU_calc_buffer[6];
    unsigned char LIJU_Buffer[LIJU_Buffer_Length];//缓冲区
    float data[6];  //浮点数结果
} _SaveData;
_SaveData Save_Data;



void Compon_data(unsigned long* desc,unsigned char* buffer,int i);
float buma_to_yuanma(unsigned long value);


int main(int argc,char** argv)
{

    //初始化串口
	int fd,ret;
	struct termios old_cfg={0}; //用于保存终端配置之前的参数
	struct termios new_cfg={0}; //用于保存终端新配置的参数
	speed_t speed = B19200;		//定义波特率为19200
    /*第一步，串口初始化*/
	fd = open(argv[1],O_RDWR | O_NOCTTY | O_NDELAY);//O_NOCTTY 标志，告知系统该节点不会成为进程的控制终端
        if(fd < 0)
        {
                printf("uart device open error\n");
                return -1;
        }
	ret = tcgetattr(fd, &old_cfg);
	if(ret < 0)
        {
		printf("tcgetattr error\n");
		close(fd);
		return -1;
	}
	/*第二步，配置串口参数*/
	cfmakeraw(&new_cfg);//设置为原始模式 
	new_cfg.c_cflag |= CREAD;// 使能接收 
	cfsetspeed(&new_cfg, speed);//将波特率设置为9600
	new_cfg.c_cflag &= ~CSIZE; //将数据位相关的比特位清零
	new_cfg.c_cflag |= CS8;    //将数据位数设置为8位
	new_cfg.c_cflag &= ~PARENB;
	new_cfg.c_iflag &= ~INPCK;//设置为无校验模式
	new_cfg.c_cflag &= ~CSTOPB;//将停止位设置为1位
	new_cfg.c_cc[VTIME] = 0;// 将 MIN 和 TIME 设置为 0
	new_cfg.c_cc[VMIN] = 0;
	ret = tcflush(fd, TCIOFLUSH);//清空缓冲区域

    if(ret < 0)
    {
            printf("tcflush error\n");
            return -1;
    }
    ret = tcsetattr(fd, TCSANOW, &new_cfg);//写入配置、使配置生效
    if(ret < 0){
            printf("tcsetattr error\n");
            return -1;
    }	

    while (1)
    {
        int ret = write(fd,Get_data,8);
        if(ret < 0)
        {
                printf("write fail\r\n");
        }
        else
        {
                printf("write data: %d\r\n",ret);
        }
        bzero(Save_Data.LIJU_Buffer,sizeof(Save_Data.LIJU_Buffer));
        ret = read(fd,Save_Data.LIJU_Buffer,LIJU_Buffer_Length);
        
        if(ret < 0)
        {
            printf("read fail\r\n");
        }
        else
        {
#ifdef  __Debug_info
            printf("read data: %d\r\n",ret);
            for(int i = 0;i < ret;i++)
            {
                    printf("0x%x ",Save_Data.LIJU_Buffer[i]);
            }
            printf("\r\n");
#endif
			//分离数据
            Compon_data(&Save_Data.Mx,Save_Data.LIJU_Buffer,1);
			Save_Data.LIJU_calc_buffer[3] = Save_Data.Mx;
            Compon_data(&Save_Data.My,Save_Data.LIJU_Buffer,2);
			Save_Data.LIJU_calc_buffer[4] = Save_Data.My;
            Compon_data(&Save_Data.Mz,Save_Data.LIJU_Buffer,3);
			Save_Data.LIJU_calc_buffer[5] = Save_Data.Mz;
            Compon_data(&Save_Data.fx,Save_Data.LIJU_Buffer,4);
			Save_Data.LIJU_calc_buffer[0] = Save_Data.fx;
            Compon_data(&Save_Data.fx,Save_Data.LIJU_Buffer,5);
			Save_Data.LIJU_calc_buffer[1] = Save_Data.fy;
            Compon_data(&Save_Data.fz,Save_Data.LIJU_Buffer,6);
			Save_Data.LIJU_calc_buffer[2] = Save_Data.fz;


			float tmp;
			for (int i = 0; i < 6; i++)
			{
				if(Save_Data.LIJU_calc_buffer[i] & 0x80000000)//负数
				{
					tmp = buma_to_yuanma(Save_Data.LIJU_calc_buffer[i]);
					Save_Data.data[i] = tmp;

				}
				else//正数
				{

					Save_Data.data[i] = (float)Save_Data.LIJU_calc_buffer[i] / 100;

				}
			}
			
            // //打印相关数据
            printf("fx: %0.2f\r\n",Save_Data.data[0]);
            printf("fy: %0.2f\r\n",Save_Data.data[1]);
            printf("fz: %0.2f\r\n",Save_Data.data[2]);
            printf("Mx: %0.2f\r\n",Save_Data.data[3]);
            printf("My: %0.2f\r\n",Save_Data.data[4]);
            printf("Mz: %0.2f\r\n",Save_Data.data[5]);   
        }

        sleep(1);
    }
    

    return 0;
}


void Compon_data(unsigned long *desc,unsigned char* buffer,int i)
{
    if(i == 1)
	{
		*desc = buffer[3];
		*desc <<= 8;
		*desc |= buffer[4];
		*desc <<= 8;
		*desc |= buffer[5];
		*desc <<= 8;
		*desc |= buffer[6];
	}
	else if(i == 2)
	{
		*desc = buffer[7];
		*desc <<= 8;
		*desc |= buffer[8];
		*desc <<= 8;
		*desc |= buffer[9];
		*desc <<= 8;
		*desc |= buffer[10];
	}	
	else if(i == 3)
	{
		*desc = buffer[11];
		*desc <<= 8;
		*desc |= buffer[12];
		*desc <<= 8;
		*desc |= buffer[13];
		*desc <<= 8;
		*desc |= buffer[14];
	}
	
	else if(i == 4)
	{
		*desc = buffer[15];
		*desc <<= 8;
		*desc |= buffer[16];
		*desc <<= 8;
		*desc |= buffer[17];
		*desc <<= 8;
		*desc |= buffer[18];
	}
	
	else if(i == 5)
	{
		*desc = buffer[19];
		*desc <<= 8;
		*desc |= buffer[20];
		*desc <<= 8;
		*desc |= buffer[21];
		*desc <<= 8;
		*desc |= buffer[22];
	}
	
	else if(i == 6)
	{
		*desc = buffer[23];
		*desc <<= 8;
		*desc |= buffer[24];
		*desc <<= 8;
		*desc |= buffer[25];
		*desc <<= 8;
		*desc |= buffer[26];
	}
}

float buma_to_yuanma(unsigned long value)
{
	unsigned long buma = value;
	unsigned long fanma = 0;
	signed long yuanma = 0;
	unsigned long index, temp = 0;

	fanma = buma - 1;

	for (index = 0; index < 31; index++)
	{
		temp = fanma >> index;
		temp = ~temp;
		temp &= 0x01;
		temp = temp << index;
		yuanma += temp;
	}

	if (fanma & 0x80)
	{
		yuanma = -yuanma;
	}

	return (float)yuanma / 100;
}