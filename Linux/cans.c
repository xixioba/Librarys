#include <getopt.h>
#include <libgen.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <errno.h>

#include <net/if.h>

#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/uio.h>

#include <linux/can.h>
#include <linux/can/raw.h>

// #include "cans.h"

/*
设置CAN口波特率
port：can口号（ARM板0,1代表can0,can1）
bitrate：波特率，单位K
*/
// void set_baudrate(int port, int bitrate)//
// {
// 	char cmd[64];
// 	sprintf(cmd,"ifconfig can%d down",port);
// 	system(cmd);
// 	if (port==0 || port==1)
// 		sprintf(cmd,"echo %d > /sys/devices/platform/FlexCAN.%d/bitrate",bitrate*1000,port);
// 	else if(port==2 || port ==3)
// 		sprintf(cmd,"/sbin/ip link set can%d type can bitrate %d",port, bitrate*1000);

// 	system(cmd);
// 	sprintf(cmd,"ifconfig can%d up",port);	
// 	system(cmd);
// }

void set_baudrate(int port, int bitrate)//port==1,2
{
	char cmd[64];
	sprintf(cmd,"ip link set can%d down",port);
	system(cmd);
	sprintf(cmd,"ip link set can%d up type can bitrate %d",port,bitrate*1000);
	system(cmd);
	// sprintf(cmd,"echo %d > /sys/class/net/can%d/tx_queue_len",4096,port);
	// system(cmd);
	sprintf(cmd,"ip link set can%d up",port);	
	system(cmd);
}

/*
can写入
s:can描述符
data:数组首地址
num:写入字节数
*/
int Can_W(int s,int id,char *data,int num)
{
	int i,ret,t;
	struct can_frame txframe;
	txframe.can_id=id;
	txframe.can_dlc=num;
	for(i=0;i<num;i++)
	{
		txframe.data[i]=data[i];
	}
	//setsockopt(s, SOL_CAN_RAW, CAN_RAW_FILTER, NULL, 0);仅发送不接收
	// if(sendto(s, &frame,sizeof(frame),0, (struct sockaddr*)&addr, sizeof(addr)))
	// do
	// {
	// 	if(t++>500)
	// 		return -1;
	ret=write(s,&txframe,sizeof(txframe));
	return ret;
}

/*
can读取
s:can描述符
data:数组首地址
id:id地址
*/
int Can_R(int s,char *data,int *id)
{
	int n,i;
	struct can_frame rxframe;
	if(read(s,&rxframe,sizeof(rxframe))>0)
	{
		*id=rxframe.can_id;
		for(i=0;i<rxframe.can_dlc;i++)
		{
			data[i]=rxframe.data[i];			
		}
		return rxframe.can_dlc;
	}
	return -1;
}
/*
设置过滤器
s:can描述符
rfilter:can_filter结构体
struct can_filter {
	canid_t can_id;
	canid_t can_mask;
};
*/
void set_filter(int s, struct can_filter rfilter)
{
	if (setsockopt(s, SOL_CAN_RAW, CAN_RAW_FILTER, &rfilter,
				sizeof(struct can_filter)) != 0)
	{
		perror("setsockopt");
		exit(1);
	}
}

/*
can设备初始化
devname:设备名称（can0，can1），字符串
id:过滤ID，如0，则接受除ID=0外所有ID的报文
budarate:传输波特率
*/
int can_init(char *devname,unsigned long id,int budarate)
{
	int s;
	int i;
	if(strcmp(devname,"can0")==0)
	{
		// printf("can0 set buadrate\n");
		set_baudrate(0,budarate);
	}
	if(strcmp(devname,"can1")==0)
	{
		// printf("can1 set buadrate\n");
		set_baudrate(1,budarate);
	}
	s=socket(PF_CAN,SOCK_RAW,CAN_RAW);
	struct ifreq ifr;
	strcpy(ifr.ifr_name,devname);
	if(ioctl(s,SIOCGIFINDEX,&ifr))
		perror("ioctl error");
	struct can_filter rfilter;
	rfilter.can_id=id;
	rfilter.can_mask=CAN_SFF_MASK&CAN_INV_FILTER;
	if(setsockopt(s,SOL_CAN_RAW,CAN_RAW_FILTER,&rfilter,sizeof(rfilter))!=0)
		printf("set filter error\n");
	struct sockaddr_can addr;
	addr.can_family =AF_CAN;
	addr.can_ifindex=ifr.ifr_ifindex;
	if(bind(s,(struct sockaddr *)&addr,sizeof(addr))<0)
		perror("bind error");
	return s;
}

