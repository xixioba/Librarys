#ifndef __WEBSOCKET_H
#define __WEBSOCKET_H 

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#ifdef _WIN32
    #include <Winsock2.h>
    #include <windows.h>
    #include <iostream>
    #include <thread>
    #define Delay(x) Sleep(1000*x);
   #ifdef _WIN64
      //define something for Windows (64-bit only)
   #else
      //define something for Windows (32-bit only)
   #endif
#elif __linux__
    #include <sys/types.h>
    #include <sys/stat.h>
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <ctype.h>
    #include <pthread.h>  
    #include <net/if.h>
    #define Delay(x) sleep(x);
#else
    #   error "Unknown compiler"
#endif

#include "sha1.h"
#include "base64.h"
#include "network.h"

using namespace std;

struct WebSocketStreamHeader {
	unsigned int header_size;				//数据包头大小
	int mask_offset;					//掩码偏移
	unsigned int payload_size;				//数据大小
	bool fin;                                               //帧标记
	bool masked;					        //掩码
	unsigned char opcode;					//操作码
	unsigned char res[3];					
};


enum WS_Status
{
    WS_STATUS_CONNECT = 0,
    WS_STATUS_UNCONNECT = 1,
};
 
enum WS_FrameType
{
    WS_EMPTY_FRAME = 0xF0,
    WS_ERROR_FRAME = 0xF1,
    WS_TEXT_FRAME   = 0x01,
    WS_BINARY_FRAME = 0x02,
    WS_PING_FRAME = 0x09,
    WS_PONG_FRAME = 0x0A,
    WS_OPENING_FRAME = 0xF3,
    WS_CLOSING_FRAME = 0x08
};

class Thread
{
    private:
        pthread_t pid;
    private:
        static void * start_thread(void *arg);// //静态成员函数
    public: 
        int start();
        virtual void run() = 0; //基类中的虚函数要么实现，要么是纯虚函数（绝对不允许声明不实现，也不纯虚）
};


class WEBSOCKET:public Thread,TCP
{
	using TCP::TCP;
public:
	int Send(int fd,char *data,uint32_t len=0);
	int Read(int fd,char *data,uint32_t len);
	int webfd=0;
    void run();
};

#endif