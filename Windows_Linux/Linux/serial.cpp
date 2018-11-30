#include <linux/types.h> 
#include <linux/stat.h>  
#include <string.h>       /*字符串功能函数*/
#include <termios.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>



/*
串口配置
fd:串口描述符
nSpeed:波特率
nBits:数据位
nEvent:校验
nStop:停止位
*/
static int set_serial(int fd,int nSpeed, int nBits, char nEvent, int nStop)
{
    struct termios newtio,oldtio;
    if  ( tcgetattr( fd,&oldtio)  !=  0) 
    { 
        perror("SetupSerial 1");
        return -1;
    }
    bzero( &newtio, sizeof( newtio ) );
    newtio.c_cflag  |=  CLOCAL | CREAD; 
    newtio.c_cflag &= ~CSIZE; 

    switch( nBits )
    {
    case 7:
        newtio.c_cflag |= CS7;
        break;
    case 8:
        newtio.c_cflag |= CS8;
        break;
    }

    switch( nEvent )
    {
    case 'O':                     //奇校验
        newtio.c_cflag |= PARENB;
        newtio.c_cflag |= PARODD;
        newtio.c_iflag |= (INPCK | ISTRIP);
        break;
    case 'E':                     //偶校验
        newtio.c_iflag |= (INPCK | ISTRIP);
        newtio.c_cflag |= PARENB;
        newtio.c_cflag &= ~PARODD;
        break;
    case 'N':                    //无校验
        newtio.c_cflag &= ~PARENB;
        break;
    }

switch( nSpeed )
    {
    case 2400:
        cfsetispeed(&newtio, B2400);
        cfsetospeed(&newtio, B2400);
        break;
    case 4800:
        cfsetispeed(&newtio, B4800);
        cfsetospeed(&newtio, B4800);
        break;
    case 9600:
        cfsetispeed(&newtio, B9600);
        cfsetospeed(&newtio, B9600);
        break;
    case 115200:
        cfsetispeed(&newtio, B115200);
        cfsetospeed(&newtio, B115200);
        break;
    default:
        cfsetispeed(&newtio, B9600);
        cfsetospeed(&newtio, B9600);
        break;
    }
    if( nStop == 1 )
    {
        newtio.c_cflag &=  ~CSTOPB;
    }
    else if ( nStop == 2 )
    {
        newtio.c_cflag |=  CSTOPB;
    }
    newtio.c_cc[VTIME]  = 0;
    newtio.c_cc[VMIN] = 0;
    //newtio.c_lflag|=ICANON;
    tcflush(fd,TCIFLUSH);
    if((tcsetattr(fd,TCSANOW,&newtio))!=0)
    {
        perror("com set error");
        return -1;
    }
    return 0;
}


/*
串口读取
fd:串口描述符
data:数组地址
len:数据长度
*/
int Serial_W(int fd,char * data)
{
	return write(fd,data,strlen(data));
}


int Serial_W_RAW(int fd,char * data,int len)
{
    return write(fd,data,len);
}
/*
串口读取
fd:串口描述符
data:数组地址
*/
int Serial_R(int fd,char * data,int len)
{
    // memset(data,0,sizeof(data));
    return read(fd,data,len);
}

/*
串口设备初始化
devname:设备名称
baudrate:波特率
*/

int Serial_init(char *devname,int baudrate)
{
    int fd;
    fd=open(devname,O_RDWR | O_NOCTTY|O_NDELAY);
    if(fd<0)
    {
        printf("can not open %s\n",devname);
        return -1;
    }
    if(set_serial(fd,baudrate,8,'N',1)<0)
    {
        printf("can not set serial\n");
        return -1;
    }
    return fd;
}


