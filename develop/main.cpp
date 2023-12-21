#include "HotPlug.h"
 
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <termios.h>
#include <stdbool.h>
#include <map>
#include <iostream>
#include <thread>
#include <string>
using namespace std;


/*******************************力矩***************************************/
/*******************************hyxw***************************************/
//获取解耦数据
unsigned char Get_data[8] = {0x01,0x03,0x00,0xD4,0x00,0x0C,0x05,0xF7};
 #define hyxw_Buffer_Length 30
typedef struct hyxw_SaveData
{
    unsigned long fx;//x轴力
    unsigned long fy;//y轴力
    unsigned long fz;//z轴力
    unsigned long Mx;//x轴力矩
    unsigned long My;//y轴力矩
    unsigned long Mz;//z轴力矩
	unsigned long LIJU_calc_buffer[6];
    unsigned char LIJU_Buffer[hyxw_Buffer_Length];//缓冲区
    float data[6];  //浮点数结果
} hyxw_SaveData;
hyxw_SaveData hyxw_Save_Data;
void Compon_data(unsigned long* desc,unsigned char* buffer,int i);
float buma_to_yuanma(unsigned long value);
void cb_hyxw_test();//
void cb_hyxw_test_singal();

/****************************速度*********************************************/
/****************************brt*********************************************/
#define brt_Buffer_Length 60
typedef struct BRT_SaveData
{
        char SUDU_Buffer[brt_Buffer_Length];
        char order[8]; 
        char buf[5] = { 0 };
        int Data;
        int  pos = 0;
        float n = 0;  //转速
} BRT_SaveData;
BRT_SaveData Brt_Save_Data;
int getIndexOfSigns(char ch);
long hexToDec(char* source);
void cb_brt_test();
void cb_brt_test_singal();

/****************************全局*********************************************/
void observeALLDeviceHotPlugEventCallback(const DevType devType,const DevAction devAction,const char * devPath);
/****************************hyxw*********************************************/
void observe_HYXW_HotPlugEventCallback(const DevType devType,const DevAction devAction,const char * devPath);
/****************************brt*********************************************/
void observe_BRT_HotPlugEventCallback(const DevType devType,const DevAction devAction,const char * devPath);
 
//尾插
int arry_insert_back(const char* devpath);
//删除
void array_delete_pre(const char* devpath);


//是否打开调试信息
// #define __Debug_info

//同时支持的设备数目
#define dev_num 10

string port[dev_num] = {""};
int fd[dev_num] = {-1};
bool isstop[dev_num] = {false};
bool isonline[dev_num] = {false};

typedef void (*function_type)();
std::map<const char*,int> pos_map;
std::map<const char*,function_type> dev_map;

int main(int argc , char * argv[])
{
    if(argc < 1 || argc >= 2)
    {
        if(strcmp(argv[1],"-h") == 0)
        {
            printf("-hyxw   支持力矩传感器hyxw-x6热插拔\r\n");
            printf("-brt    支持速度传感器brt热插拔\r\n");
            printf("-all    支持插入多个传感器\r\n");
            printf("-all    支持插入多个传感器\r\n");
            printf("-all    支持插入多个传感器(按一定顺序)\r\n");

            exit(-1);
        }
        else if(strcmp(argv[1],"-all") == 0)
        {
            //注册设备回调表
            dev_map.insert(std::pair<const char*,function_type>("/dev/ttyUSB0",cb_hyxw_test));
            dev_map.insert(std::pair<const char*,function_type>("/dev/ttyUSB1",cb_brt_test));
#ifdef __Debug_info

            for(map<const char*,function_type>::iterator it=dev_map.begin();it!=dev_map.end();it++)
            {
                cout <<"[dev_map]: " << "key:" <<it->first<<" value:"<<it->second<<endl;
            }
#endif 
            //注册设备号相对位置表
            pos_map.insert(std::pair<const char*,int>("/dev/ttyUSB0",0));
            pos_map.insert(std::pair<const char*,int>("/dev/ttyUSB1",1));

#ifdef __Debug_info
            for(map<const char*,int>::iterator it=pos_map.begin();it!=pos_map.end();it++)
            {
                cout << "[pos_map]: "<< "key:" <<it->first<<" pos:"<<it->second<<endl;
            } 
#endif             
            //初始化热插拔服务器
            initHotPlugObserver();
        
            //注册热插拔事件回调
            registerObserveCallback(ObserveDeviceType_All,observeALLDeviceHotPlugEventCallback);
        
#if 0
            //注销热插拔服务器
            unInitHotPlugObserver();
        
            //注销热插拔事件回调
            unregisterObserveCallback(ObserveDeviceType_Block,observeBlockDeviceHotPlugEventCallback);    

#endif   
            printf("系统初始化完毕\r\n");

            //阻塞主线程
            pause();                
        } 
        else if(strcmp(argv[1],"-hyxw") == 0)
        {
            //注册设备回调表
            dev_map.insert(std::pair<const char*,function_type>("/dev/ttyUSB0",cb_hyxw_test_singal));
            //注册设备号相对位置表
            pos_map.insert(std::pair<const char*,int>("/dev/ttyUSB0",0));
            //初始化热插拔服务器
            initHotPlugObserver();
            //注册热插拔事件回调
            registerObserveCallback(ObserveDeviceType_All,observe_HYXW_HotPlugEventCallback);
            printf("系统初始化完毕\r\n");

            //阻塞主线程
            pause();    
        }
        else if(strcmp(argv[1],"-brt") == 0)
        {
            //注册设备回调表
            dev_map.insert(std::pair<const char*,function_type>("/dev/ttyUSB0",cb_brt_test_singal));
            //注册设备号相对位置表
            pos_map.insert(std::pair<const char*,int>("/dev/ttyUSB0",0));
            //初始化热插拔服务器
            initHotPlugObserver();
            //注册热插拔事件回调
            registerObserveCallback(ObserveDeviceType_All,observe_BRT_HotPlugEventCallback);
            printf("系统初始化完毕\r\n");

            //阻塞主线程
            pause();
            exit(-1);
        }
        else
        {
            printf("\r\n请输入正确的参数!!! -h获取可用参数\r\n\r\n");    
            exit(-1);
        }       
    }
    else
    {
        printf("\r\n请输入正确的参数!!! -h获取可用参数\r\n\r\n");
        exit(-1);
    }
    return 0;
}
/*********************************全局************************************************/
void observeALLDeviceHotPlugEventCallback(const DevType devType,const DevAction devAction,const char * devPath)
{
    // printf(" observeBlockDeviceHotPlugEventCallback devType=%d devAction=%d devPath=%s ",devType,devAction,devPath);
    
    int pos = arry_insert_back(devPath);
    printf("\r\npos = %d\r\n",pos);

    if(pos != -1)
    {
        if(devType == DevType_Tty)
        {
            if(devAction == DevAction_Add)
            {
                if(strcmp(devPath,"/dev/ttyUSB0") == 0)
                {

                    auto iter = pos_map.find("/dev/ttyUSB0");
                    if(iter != pos_map.end())
                    {
                        isstop[iter->second] = false; 
                        auto iter2 = dev_map.find("/dev/ttyUSB0");
                        if(iter2 != dev_map.end())
                        {
                            std::thread hyxw(iter2->second);
                            hyxw.detach();
                            printf("力矩传感器hyxw-x6插入\r\n");
                        }
                        else
                        {
                            printf("[ADD]: hyxw dev_map没找到 %d\r\n",__LINE__);
                        }
                    }
                    else
                    {
                        printf("[ADD]:hyxw pos_map没找到 %d\r\n",__LINE__);
                    }
                }  
                else if(strcmp(devPath,"/dev/ttyUSB1") == 0)
                {
                    auto iter = pos_map.find("/dev/ttyUSB1");
                    if(iter != pos_map.end())
                    {
                        isstop[iter->second] = false; 
                        auto iter2 = dev_map.find("/dev/ttyUSB1");
                        if(iter2 != dev_map.end())
                        {
                            std::thread brt(iter2->second);
                            brt.detach();
                            printf("速度传感器brt插入\r\n");
                        }
                        else
                        {
                            printf("[ADD]: brt dev_map没找到 %d\r\n",__LINE__);
                        }
                    }
                    else
                    {
                        printf("[ADD]:brt pos_map没找到 %d\r\n",__LINE__);
                    }
                }  
            }
            else if(devAction == DevAction_Remove)
            {
                if(strcmp(devPath,"/dev/ttyUSB0") == 0)
                {
                    auto iter = pos_map.find("/dev/ttyUSB0");
                    if(iter != pos_map.end())
                    {
                        isstop[iter->second] = true;   
                        
                        array_delete_pre(devPath);
                        printf("力矩传感器 hyxw-x6拔出\r\n");
                    }
                    else
                    {
                        printf("[REMOVE]:pos_map没找到 %d\r\n",__LINE__);   
                    }
                }
                else if(strcmp(devPath,"/dev/ttyUSB1") == 0)
                {
                    if(strcmp(devPath,"/dev/ttyUSB1") == 0)
                    {
                        auto iter = pos_map.find("/dev/ttyUSB1");
                        if(iter != pos_map.end())
                        {
                            isstop[iter->second] = true;   
                           
                            array_delete_pre(devPath);
                            printf("速度传感器 brt拔出\r\n");
                        }
                        else
                        {
                            printf("[REMOVE]:pos_map没找到 %d\r\n",__LINE__);   
                        }
                    }
                } 
            }
        }
        //获取设备数
        int num1 = 0;
        for (int i = 0; i < dev_num; i++)
        {       
            if(port[i] == "")
            {
                continue;
            }
            else
            {
                num1++;
                cout << "dev: " << port[i] << endl;
            }
        }
        printf("\r\n设备数:%d\r\n",num1);
    }
}
/*********************************hyxw************************************************/
void observe_HYXW_HotPlugEventCallback(const DevType devType,const DevAction devAction,const char * devPath)
{
    if(strcmp(devPath,"/dev/ttyUSB0") == 0)
    {
        int pos = arry_insert_back(devPath);
        if(pos != -1)
        {
            if(devType == DevType_Tty)
            {
                if(devAction == DevAction_Add)
                {
                    if(strcmp(devPath,"/dev/ttyUSB0") == 0)
                    {
                        auto iter = pos_map.find("/dev/ttyUSB0");
                        if(iter != pos_map.end())
                        {
                            isstop[iter->second] = false; 
                            auto iter2 = dev_map.find("/dev/ttyUSB0");
                            if(iter2 != dev_map.end())
                            {
                                std::thread hyxw(iter2->second);
                                hyxw.detach();
                                printf("力矩传感器hyxw-x6插入\r\n");
                            }
                            else
                            {
                                printf("[ADD]: hyxw dev_map没找到 %d\r\n",__LINE__);
                            }
                        }
                        else
                        {
                            printf("[ADD]:hyxw pos_map没找到 %d\r\n",__LINE__);
                        }
                    }  
                }
                else if(devAction == DevAction_Remove)
                {
                    if(strcmp(devPath,"/dev/ttyUSB0") == 0)
                    {
                        auto iter = pos_map.find("/dev/ttyUSB0");
                        if(iter != pos_map.end())
                        {
                            isstop[iter->second] = true; 
                              
                            array_delete_pre(devPath);
                            printf("力矩传感器 hyxw-x6拔出\r\n");
                        }
                        else
                        {
                            printf("[REMOVE]:pos_map没找到 %d\r\n",__LINE__);   
                        }
                    }
                }
            }
        }
    }
}
/*********************************brt************************************************/
void observe_BRT_HotPlugEventCallback(const DevType devType,const DevAction devAction,const char * devPath)
{
    if(strcmp(devPath,"/dev/ttyUSB0") == 0)
    {
        int pos = arry_insert_back(devPath);
        if(pos != -1)
        {
            if(devType == DevType_Tty)
            {
                if(devAction == DevAction_Add)
                {
                    if(strcmp(devPath,"/dev/ttyUSB0") == 0)
                    {
                        auto iter = pos_map.find("/dev/ttyUSB0");
                        if(iter != pos_map.end())
                        {
                            isstop[iter->second] = false; 
                            auto iter2 = dev_map.find("/dev/ttyUSB0");
                            if(iter2 != dev_map.end())
                            {
                                std::thread brt(iter2->second);
                                brt.detach();
                                printf("速度传感器brt插入\r\n");
                            }
                            else
                            {
                                printf("[ADD]: brt dev_map没找到 %d\r\n",__LINE__);
                            }
                        }
                        else
                        {
                            printf("[ADD]:brt pos_map没找到 %d\r\n",__LINE__);
                        }
                    }  
                }
                else if(devAction == DevAction_Remove)
                {
                    if(strcmp(devPath,"/dev/ttyUSB0") == 0)
                    {
                        auto iter = pos_map.find("/dev/ttyUSB0");
                        if(iter != pos_map.end())
                        {
                            isstop[iter->second] = true;   
                            
                            array_delete_pre(devPath);
                            printf("速度传感器 brt拔出\r\n");
                        }
                        else
                        {
                            printf("[REMOVE]:pos_map没找到 %d\r\n",__LINE__);   
                        }
                    }
                }
            }
        }
    }
}
//插
int arry_insert_back(const char* devpath)
{
    int num = 0;
    for (int i = 0; i < dev_num; i++)
    {       
        if(port[i] == "")
        {
            continue;
        }
        else
        {
            num++;
        }
    }
    if(num >= dev_num)
    {
        printf("设备太多\r\n");
        exit(-1);
    }

    if(strcmp(devpath,"/dev/ttyUSB0") == 0)
    {
        port[0] = devpath;
        return 0;
    }
    else if(strcmp(devpath,"/dev/ttyUSB1") == 0)
    {
        port[1] = devpath;
        return 1;
    }
    else if(strcmp(devpath,"/dev/ttyUSB2") == 0)
    {
        port[2] = devpath;
        return 2;
    }

    return -1;
}
//删
void array_delete_pre(const char* devpath)
{
    if(strcmp(devpath,"/dev/ttyUSB0") == 0)
    {
        port[0] = "";
       
    }
    else if(strcmp(devpath,"/dev/ttyUSB1") == 0)
    {
        port[1] = "";
        
    }
    else if(strcmp(devpath,"/dev/ttyUSB2") == 0)
    {
        port[2] = "";
       
    }

}
/*********************************力矩************************************************/
/*********************************hyxw************************************************/
void cb_hyxw_test()
{
    printf("\r\ncb_hyxw_test thread :%ld\r\n",pthread_self());
    //初始化串口
	int ret,pos = -1;
	struct termios old_cfg={0}; //用于保存终端配置之前的参数
	struct termios new_cfg={0}; //用于保存终端新配置的参数
	speed_t speed = B19200;		//定义波特率为19200
    /*第一步，串口初始化*/
    for(map<const char*,int>::iterator it=pos_map.begin();it!=pos_map.end();it++)
    {
        if(strcmp(it->first,"/dev/ttyUSB0") == 0)
        {
            pos = it->second;
            break;
        }
        else
        {
            printf("cb_hyxw_test pos_map没找到\r\n");
        }
    }
	fd[pos] = open(port[pos].c_str(),O_RDWR | O_NOCTTY | O_NDELAY);//O_NOCTTY 标志，告知系统该节点不会成为进程的控制终端
	if(fd[pos] < 0)
	{
			printf("uart device open error\n");
			return;
	}
	ret = tcgetattr(fd[pos], &old_cfg);
	if(ret < 0)
        {
		printf("tcgetattr error\n");
		close(fd[pos]);
		return ;
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
	ret = tcflush(fd[pos], TCIOFLUSH);//清空缓冲区域

    if(ret < 0)
    {
        printf("tcflush error\n");
        return;
    }
    ret = tcsetattr(fd[pos], TCSANOW, &new_cfg);//写入配置、使配置生效
    if(ret < 0)
    {
            printf("tcsetattr error\n");
            return ;
    }	

    while (isstop[pos] != true)
    {
        int ret = write(fd[pos],Get_data,8);
        if(ret < 0)
        {
                printf("write fail%d\r\n",__LINE__);
        }
        else
        {
                printf("write data: %d\r\n",ret);
        }
        bzero(hyxw_Save_Data.LIJU_Buffer,sizeof(hyxw_Save_Data.LIJU_Buffer));
        ret = read(fd[pos],hyxw_Save_Data.LIJU_Buffer,hyxw_Buffer_Length);
        
        if(ret < 0)
        {
            printf("read fail %d\r\n",__LINE__);
            sleep(1);
        }
        else
        {
#ifdef  __Debug_info
            printf("read data: %d\r\n",ret);
            for(int i = 0;i < ret;i++)
            {
                    printf("0x%x ",hyxw_Save_Data.LIJU_Buffer[i]);
            }
            printf("\r\n");
#endif
			//分离数据
            Compon_data(&hyxw_Save_Data.Mx,hyxw_Save_Data.LIJU_Buffer,1);
			hyxw_Save_Data.LIJU_calc_buffer[3] = hyxw_Save_Data.Mx;
            Compon_data(&hyxw_Save_Data.My,hyxw_Save_Data.LIJU_Buffer,2);
			hyxw_Save_Data.LIJU_calc_buffer[4] = hyxw_Save_Data.My;
            Compon_data(&hyxw_Save_Data.Mz,hyxw_Save_Data.LIJU_Buffer,3);
			hyxw_Save_Data.LIJU_calc_buffer[5] = hyxw_Save_Data.Mz;
            Compon_data(&hyxw_Save_Data.fx,hyxw_Save_Data.LIJU_Buffer,4);
			hyxw_Save_Data.LIJU_calc_buffer[0] = hyxw_Save_Data.fx;
            Compon_data(&hyxw_Save_Data.fx,hyxw_Save_Data.LIJU_Buffer,5);
			hyxw_Save_Data.LIJU_calc_buffer[1] = hyxw_Save_Data.fy;
            Compon_data(&hyxw_Save_Data.fz,hyxw_Save_Data.LIJU_Buffer,6);
			hyxw_Save_Data.LIJU_calc_buffer[2] = hyxw_Save_Data.fz;


			float tmp;
			for (int i = 0; i < 6; i++)
			{
				if(hyxw_Save_Data.LIJU_calc_buffer[i] & 0x80000000)//负数
				{
					tmp = buma_to_yuanma(hyxw_Save_Data.LIJU_calc_buffer[i]);
					hyxw_Save_Data.data[i] = tmp;

				}
				else//正数
				{

					hyxw_Save_Data.data[i] = (float)hyxw_Save_Data.LIJU_calc_buffer[i] / 100;

				}
			}
			
            // //打印相关数据
            printf("fx: %0.2f\r\n",hyxw_Save_Data.data[0]);
            printf("fy: %0.2f\r\n",hyxw_Save_Data.data[1]);
            printf("fz: %0.2f\r\n",hyxw_Save_Data.data[2]);
            printf("Mx: %0.2f\r\n",hyxw_Save_Data.data[3]);
            printf("My: %0.2f\r\n",hyxw_Save_Data.data[4]);
            printf("Mz: %0.2f\r\n",hyxw_Save_Data.data[5]);   
        }

        sleep(1);
    }
    printf("hyxw线程退出\r\n");
    tcsetattr(fd[pos], TCSANOW, &old_cfg);//恢复到之前的配置
    close(fd[pos]);
}
void cb_hyxw_test_singal()
{
    //初始化串口
	int ret,pos = -1;
	struct termios old_cfg={0}; //用于保存终端配置之前的参数
	struct termios new_cfg={0}; //用于保存终端新配置的参数
	speed_t speed = B19200;		//定义波特率为19200
    /*第一步，串口初始化*/
    for(map<const char*,int>::iterator it=pos_map.begin();it!=pos_map.end();it++)
    {
        if(strcmp(it->first,"/dev/ttyUSB0") == 0)
        {
                pos = it->second;
                break;
        }
        else
        {
                printf("cb_hyxw_test pos_map没找到\r\n");
        }
    }
	fd[pos] = open(port[pos].c_str(),O_RDWR | O_NOCTTY | O_NDELAY);//O_NOCTTY 标志，告知系统该节点不会成为进程的控制终端
	if(fd[pos] < 0)
	{
			printf("uart device open error\n");
			return;
	}
	ret = tcgetattr(fd[pos], &old_cfg);
	if(ret < 0)
        {
		printf("tcgetattr error\n");
		close(fd[pos]);
		return ;
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
	ret = tcflush(fd[pos], TCIOFLUSH);//清空缓冲区域

    if(ret < 0)
    {
        printf("tcflush error\n");
        return;
    }
    ret = tcsetattr(fd[pos], TCSANOW, &new_cfg);//写入配置、使配置生效
    if(ret < 0)
    {
            printf("tcsetattr error\n");
            return ;
    }	

    while (isstop[pos] != true)
    {
        int ret = write(fd[pos],Get_data,8);
        if(ret < 0)
        {
                printf("write fail%d\r\n",__LINE__);
        }
        else
        {
                printf("write data: %d\r\n",ret);
        }
        bzero(hyxw_Save_Data.LIJU_Buffer,sizeof(hyxw_Save_Data.LIJU_Buffer));
        ret = read(fd[pos],hyxw_Save_Data.LIJU_Buffer,hyxw_Buffer_Length);
        
        if(ret < 0)
        {
            printf("read fail %d\r\n",__LINE__);
            sleep(1);
        }
        else
        {
#ifdef  __Debug_info
            printf("read data: %d\r\n",ret);
            for(int i = 0;i < ret;i++)
            {
                    printf("0x%x ",hyxw_Save_Data.LIJU_Buffer[i]);
            }
            printf("\r\n");
#endif
			//分离数据
            Compon_data(&hyxw_Save_Data.Mx,hyxw_Save_Data.LIJU_Buffer,1);
			hyxw_Save_Data.LIJU_calc_buffer[3] = hyxw_Save_Data.Mx;
            Compon_data(&hyxw_Save_Data.My,hyxw_Save_Data.LIJU_Buffer,2);
			hyxw_Save_Data.LIJU_calc_buffer[4] = hyxw_Save_Data.My;
            Compon_data(&hyxw_Save_Data.Mz,hyxw_Save_Data.LIJU_Buffer,3);
			hyxw_Save_Data.LIJU_calc_buffer[5] = hyxw_Save_Data.Mz;
            Compon_data(&hyxw_Save_Data.fx,hyxw_Save_Data.LIJU_Buffer,4);
			hyxw_Save_Data.LIJU_calc_buffer[0] = hyxw_Save_Data.fx;
            Compon_data(&hyxw_Save_Data.fx,hyxw_Save_Data.LIJU_Buffer,5);
			hyxw_Save_Data.LIJU_calc_buffer[1] = hyxw_Save_Data.fy;
            Compon_data(&hyxw_Save_Data.fz,hyxw_Save_Data.LIJU_Buffer,6);
			hyxw_Save_Data.LIJU_calc_buffer[2] = hyxw_Save_Data.fz;


			float tmp;
			for (int i = 0; i < 6; i++)
			{
				if(hyxw_Save_Data.LIJU_calc_buffer[i] & 0x80000000)//负数
				{
					tmp = buma_to_yuanma(hyxw_Save_Data.LIJU_calc_buffer[i]);
					hyxw_Save_Data.data[i] = tmp;

				}
				else//正数
				{

					hyxw_Save_Data.data[i] = (float)hyxw_Save_Data.LIJU_calc_buffer[i] / 100;

				}
			}
			
            // //打印相关数据
            printf("fx: %0.2f\r\n",hyxw_Save_Data.data[0]);
            printf("fy: %0.2f\r\n",hyxw_Save_Data.data[1]);
            printf("fz: %0.2f\r\n",hyxw_Save_Data.data[2]);
            printf("Mx: %0.2f\r\n",hyxw_Save_Data.data[3]);
            printf("My: %0.2f\r\n",hyxw_Save_Data.data[4]);
            printf("Mz: %0.2f\r\n",hyxw_Save_Data.data[5]);   
        }

        sleep(1);
    }
    
    close(fd[pos]);
    // tcsetattr(fd[pos], TCSANOW, &old_cfg);//恢复到之前的配置
}
//分离数据
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
//补码转原码
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
/***********************************速度**************************************************/
/***********************************BRT**************************************************/
void cb_brt_test()
{
    printf("\r\ncb_brt_test thread :%ld\r\n",pthread_self());

    int ret,pos = -1;
	struct termios old_cfg; //用于保存终端配置之前的参数
	struct termios new_cfg; //用于保存终端新配置的参数
	speed_t speed = B9600;		//定义波特率为9600
    /*第一步，串口初始化*/

    auto it = pos_map.find("/dev/ttyUSB1");
    if (it != pos_map.end())
    {
        pos = it->second;
    }
    else
    {
        printf("cb_brt_test pos_map没找到 %d\r\n",__LINE__);
    }
    
	fd[pos] = open(port[pos].c_str(),O_RDWR | O_NOCTTY | O_NDELAY);//O_NOCTTY 标志，告知系统该节点不会成为进程的控制终端
    if(fd[pos] < 0)
    {
            printf("uart device open error\n");
            return;
    }
	ret = tcgetattr(fd[pos], &old_cfg);
	if(ret < 0)
        {
		printf("tcgetattr error\n");
		close(fd[pos]);
		return;
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
	ret = tcflush(fd[pos], TCIOFLUSH);//清空缓冲区域

        if(ret < 0)
        {
            printf("tcflush error\n");
            return;
        }
        ret = tcsetattr(fd[pos], TCSANOW, &new_cfg);//写入配置、使配置生效
        if(ret < 0)
        {
            printf("tcsetattr error\n");
            return;
        }	
        Brt_Save_Data.order[0] = 0x01;
        Brt_Save_Data.order[1] = 0x03;
        Brt_Save_Data.order[2] = 0x00;
        Brt_Save_Data.order[3] = 0x20;
        Brt_Save_Data.order[4] = 0x00;
        Brt_Save_Data.order[5] = 0x02;
        Brt_Save_Data.order[6] = 0xC5;
        Brt_Save_Data.order[7] = 0xC1;
	
        while(isstop[pos] != true)
        { 
            int ret = write(fd[pos],Brt_Save_Data.order,8);
            if(ret < 0)
            {
                printf("write fail %d\r\n",__LINE__);
                sleep(1);
            }
            else
            {
                printf("write data: %d pos :%d\r\n",ret,pos);
                bzero(Brt_Save_Data.SUDU_Buffer,sizeof(Brt_Save_Data.SUDU_Buffer));
                ret = read(fd[pos],Brt_Save_Data.SUDU_Buffer,10);
                if(ret < 0)
                {
                        printf("read fail %d\r\n",__LINE__);
                }
                else
                {
                    printf("read data: %d %d\r\n",ret,__LINE__);
#ifdef  __Debug_info
                    for(int i = 0;i < ret;i++)
                    {
                            printf("0x%x ",Brt_Save_Data.SUDU_Buffer[i]);
                    }
                    printf("\r\n");
#endif

                    for(int i = 0;i < ret;i++)
                    {
                        if(Brt_Save_Data.SUDU_Buffer[i] == 0x03 && Brt_Save_Data.SUDU_Buffer[i + 1] == 0x04)
                        {
                                Brt_Save_Data.pos = i + 1;
                                break;
                        }
                    }
                    sprintf(Brt_Save_Data.buf, "%x%x%x%x",Brt_Save_Data.SUDU_Buffer[ Brt_Save_Data.pos + 1],Brt_Save_Data.SUDU_Buffer[ Brt_Save_Data.pos + 2],Brt_Save_Data.SUDU_Buffer[ Brt_Save_Data.pos +3],Brt_Save_Data.SUDU_Buffer[ Brt_Save_Data.pos + 4]);

                    Brt_Save_Data.Data =  hexToDec(Brt_Save_Data.buf);
                    printf("SUDU_Data: %d\r\n", Brt_Save_Data.Data);
                    Brt_Save_Data.n =   (float)Brt_Save_Data.Data*0.00915;
                    printf("n: %f\r\n",Brt_Save_Data.n);
                }

                sleep(1);
                printf("\r\n");                    
            }
        }
        printf("brt线程退出\r\n");
        tcsetattr(fd[pos], TCSANOW, &old_cfg);//恢复到之前的配置
        close(fd[pos]);

}
void cb_brt_test_singal()
{
    int ret,pos = -1;
	struct termios old_cfg={0}; //用于保存终端配置之前的参数
	struct termios new_cfg={0}; //用于保存终端新配置的参数
	speed_t speed = B9600;		//定义波特率为9600
    /*第一步，串口初始化*/
    for(map<const char*,int>::iterator it=pos_map.begin();it!=pos_map.end();it++)
    {
        if(strcmp(it->first,"/dev/ttyUSB0") == 0)
        {
            pos = it->second;
            break;
        }
        else
        {
            printf("cb_brt_test pos_map没找到 %d\r\n",__LINE__);
        }
    }

	fd[pos] = open(port[pos].c_str(),O_RDWR | O_NOCTTY | O_NDELAY);//O_NOCTTY 标志，告知系统该节点不会成为进程的控制终端
    if(fd[pos] < 0)
    {
            printf("uart device open error\n");
            return;
    }
	ret = tcgetattr(fd[pos], &old_cfg);
	if(ret < 0)
        {
		printf("tcgetattr error\n");
		close(fd[pos]);
		return;
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
	ret = tcflush(fd[pos], TCIOFLUSH);//清空缓冲区域

        if(ret < 0)
        {
            printf("tcflush error\n");
            return;
        }
        ret = tcsetattr(fd[pos], TCSANOW, &new_cfg);//写入配置、使配置生效
        if(ret < 0)
        {
            printf("tcsetattr error\n");
            return;
        }	
        Brt_Save_Data.order[0] = 0x01;
        Brt_Save_Data.order[1] = 0x03;
        Brt_Save_Data.order[2] = 0x00;
        Brt_Save_Data.order[3] = 0x20;
        Brt_Save_Data.order[4] = 0x00;
        Brt_Save_Data.order[5] = 0x02;
        Brt_Save_Data.order[6] = 0xC5;
        Brt_Save_Data.order[7] = 0xC1;
	
        while(isstop[pos] != true)
        { 
            int ret = write(fd[pos],Brt_Save_Data.order,8);
            if(ret < 0)
            {
                printf("write fail %d\r\n",__LINE__);
                sleep(1);
            }
            else
            {
                printf("write data: %d\r\n",ret);
                bzero(Brt_Save_Data.SUDU_Buffer,sizeof(Brt_Save_Data.SUDU_Buffer));
                ret = read(fd[pos],Brt_Save_Data.SUDU_Buffer,10);
                if(ret < 0)
                {
                        printf("read fail %d\r\n",__LINE__);
                }
                else
                {
                    printf("read data: %d\r\n",ret);
#ifdef  __Debug_info
                    for(int i = 0;i < ret;i++)
                    {
                            printf("0x%x ",Brt_Save_Data.SUDU_Buffer[i]);
                    }
                    printf("\r\n");
#endif

                    for(int i = 0;i < ret;i++)
                    {
                        if(Brt_Save_Data.SUDU_Buffer[i] == 0x03 && Brt_Save_Data.SUDU_Buffer[i + 1] == 0x04)
                        {
                                Brt_Save_Data.pos = i + 1;
                                break;
                        }
                    }
                    sprintf(Brt_Save_Data.buf, "%x%x%x%x",Brt_Save_Data.SUDU_Buffer[ Brt_Save_Data.pos + 1],Brt_Save_Data.SUDU_Buffer[ Brt_Save_Data.pos + 2],Brt_Save_Data.SUDU_Buffer[ Brt_Save_Data.pos +3],Brt_Save_Data.SUDU_Buffer[ Brt_Save_Data.pos + 4]);

                    Brt_Save_Data.Data =  hexToDec(Brt_Save_Data.buf);
                    printf("SUDU_Data: %d\r\n", Brt_Save_Data.Data);
                    Brt_Save_Data.n =   (float)Brt_Save_Data.Data*0.00915;
                    printf("n: %f\r\n",Brt_Save_Data.n);
                }

                sleep(1);
                printf("\r\n");                    
            }
        }
        
        tcsetattr(fd[pos], TCSANOW, &old_cfg);//恢复到之前的配置
        close(fd[pos]);

}
/* 返回ch字符在sign数组中的序号 */
int getIndexOfSigns(char ch)
{
    if (ch >= '0' && ch <= '9')
    {
        return ch - '0';
    }
    if (ch >= 'A' && ch <= 'F')
    {
        return ch - 'A' + 10;
    }
    if (ch >= 'a' && ch <= 'f')
    {
        return ch - 'a' + 10;
    }
    return -1;

}
/* 十六进制数转换为十进制数 */
long hexToDec(char* source)
{
    long sum = 0;
    long t = 1;
    int i, len;

    len = (int)strlen(source);
    printf("len:%d\r\n",len);
    for (i = len - 1; i >= 0; i--)
    {
        sum += t * getIndexOfSigns(*(source + i));
        t *= 16;
    }

    return sum;
}
