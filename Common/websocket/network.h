#ifndef __NETWORK_
#define __NETWORK_
//mingw:add -lwsock32 for opject

#define _MSVC 1

#ifdef _WIN32
    #include <winsock.h>
    #include <Windows.h>
    #if _MSVC
        #pragma comment(lib,"ws2_32.lib")
        #define close closesocket
    #endif
    #include <iostream>
    #include <thread>
    #define Delay(x) Sleep(1000*x);
   #ifdef _WIN64
      //define something for Windows (64-bit only)
   #else
      //define something for Windows (32-bit only)
   #endif
#elif defined  __linux__
    #include <sys/time.h>
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
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
class UDP
{
	void *ptr;
public:
	UDP(void);
	~UDP(void);
	int Bind(int port);
	int Send(char *data,int len,char * ip,int port);
	int Read(char *data,int len);
};

class TCP
{
	void *ptr;
public:
    TCP(void);
	~TCP(void);
	int Bind(int port);
	int Listen(int max=5);
	int Accept();
	int Connect(int port,char *ip);
	int Send(int fd,char *data,int len);
	int Read(int fd,char *data,int len);
};
#endif
