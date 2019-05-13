#ifndef __NETWORK_
#define __NETWORK_
//mingw:add -lwsock32 for opject

#ifdef _WIN32
    #include <Windows.h>
    #include <winsock.h>
    #ifdef _MSC_VER
        #pragma comment(lib,"ws2_32.lib")
    #endif
    #include <iostream>
    #include <thread>
    #define Delay(x) Sleep(1000*x);
    #define Delayms(x) Sleep(x);
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
    #include <unistd.h>
    #define Delay(x) sleep(x);
    #define Delayms(x) usleep(x*1000);
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
