#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#ifdef _WIN32
	#include <Winsock2.h>
	#include <windows.h>
	#include <iostream>
	#include <thread>
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
	#include <errno.h>
#else
	#   error "Unknown compiler"
#endif

#include "network.h"


struct udp_ptr
{
	struct sockaddr_in udp;
	int sock;
};

UDP::UDP(int port=0)
{
	ptr=new udp_ptr;
#ifdef _WIN32
	char opt=1;
    WORD wVersionRequested;
    WSADATA wsaData;
    wVersionRequested = MAKEWORD( 1, 1 );
    if(WSAStartup( wVersionRequested, &wsaData )!=0)
    {
	    printf("load dll error!\n");
	    return ;		
    }
#elif __linux__
	int opt=1;
#endif
	memset(&(*(udp_ptr *)ptr).udp,0,sizeof((*(udp_ptr *)ptr).udp));
	(*(udp_ptr *)ptr).sock=socket(PF_INET,SOCK_DGRAM,0);
	setsockopt((*(udp_ptr *)ptr).sock,SOL_SOCKET,SO_REUSEADDR|SO_BROADCAST,&opt,sizeof(opt));
	(*(udp_ptr *)ptr).udp.sin_family=AF_INET;
	if(port!=0)
		Bind(port);		
}
UDP::~UDP()
{
	close((*(udp_ptr *)ptr).sock);
	delete (udp_ptr *)ptr;
}
int UDP::Bind(int port)
{
	(*(udp_ptr *)ptr).udp.sin_port=htons(port);
	(*(udp_ptr *)ptr).udp.sin_addr.s_addr=htonl(INADDR_ANY);
	return bind((*(udp_ptr *)ptr).sock,(struct sockaddr *)&(*(udp_ptr *)ptr).udp,sizeof((*(udp_ptr *)ptr).udp));
}
int UDP::Send(char *data,int len,char * ip=0,int port=0)
{
	if(port!=0)
	{
		(*(udp_ptr *)ptr).udp.sin_port=htons(port);
	}
	else
	{
		if((*(udp_ptr *)ptr).udp.sin_port==0)
			return -1;
	}
	if(ip!=0)
		(*(udp_ptr *)ptr).udp.sin_addr.s_addr=inet_addr(ip);
	else
	{
		if((*(udp_ptr *)ptr).udp.sin_addr.s_addr==0)
			return -1;
	}
	return sendto((*(udp_ptr *)ptr).sock,data,len,0,(struct sockaddr *)&(*(udp_ptr *)ptr).udp,sizeof((*(udp_ptr *)ptr).udp));
}
int UDP::Read(char *data,int len)
{
	#ifdef _WIN32
	int length=sizeof(sockaddr);
	#elif __linux__
	socklen_t length=sizeof(sockaddr);
	#endif
	return recvfrom((*(udp_ptr *)ptr).sock,data,len,0,(struct sockaddr*)&(*(udp_ptr *)ptr).udp,&length);	
}
int UDP::NoBlock(void)
{
	return fcntl((*(udp_ptr *)ptr).sock, F_SETFL, fcntl((*(udp_ptr *)ptr).sock, F_GETFL, 0)|O_NONBLOCK);	
}
int UDP::Block(void)
{
	return fcntl((*(udp_ptr *)ptr).sock, F_SETFL, fcntl((*(udp_ptr *)ptr).sock, F_GETFL, 0)& ~O_NONBLOCK);	
}

struct tcp_ptr
{
	struct sockaddr_in tcp;
	int sock;
};

TCP::TCP(int port=0,int max=10)
{
	ptr=new tcp_ptr;
#ifdef _WIN32
	char opt=1;
    WORD wVersionRequested;
    WSADATA wsaData;
    wVersionRequested = MAKEWORD( 1, 1 );
    if(WSAStartup( wVersionRequested, &wsaData )!=0)
    {
	    printf("load dll error!\n");
	    return ;		
    }
#elif __linux__
	int opt=1;
#endif
	memset(&(*(tcp_ptr *)ptr).tcp,0,sizeof((*(tcp_ptr *)ptr).tcp));
	(*(tcp_ptr *)ptr).sock=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	setsockopt((*(tcp_ptr *)ptr).sock,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt));
	(*(tcp_ptr *)ptr).tcp.sin_family=AF_INET;
	if(port>0 && max>0)
	{
		Bind(port);
		Listen(max);
	}
}
TCP::~TCP()
{
	close((*(tcp_ptr *)ptr).sock);
	delete (tcp_ptr *)ptr;
}
int TCP::Bind(int port)
{
	(*(tcp_ptr *)ptr).tcp.sin_port=htons(port);
	(*(tcp_ptr *)ptr).tcp.sin_addr.s_addr=htonl(INADDR_ANY);
	return bind((*(tcp_ptr *)ptr).sock,(struct sockaddr *)&((*(tcp_ptr *)ptr).tcp),sizeof((*(tcp_ptr *)ptr).tcp));
}
int TCP::Listen(int max)
{
	return listen((*(tcp_ptr *)ptr).sock,max);
}
int TCP::Accept(void)
{
	struct sockaddr_in client;
	#ifdef _WIN32
	int length=sizeof(sockaddr);
	#elif __linux__
	socklen_t length=sizeof(sockaddr);
	#endif
	return accept((*(tcp_ptr *)ptr).sock, (struct sockaddr*)&client, &length);
}
int TCP::Connect(char *ip,int port) 
{
	(*(tcp_ptr *)ptr).tcp.sin_port=htons(port);
	(*(tcp_ptr *)ptr).tcp.sin_addr.s_addr=inet_addr(ip);
	if(connect((*(tcp_ptr *)ptr).sock, (struct sockaddr*) &(*(tcp_ptr *)ptr).tcp, sizeof((*(tcp_ptr *)ptr).tcp))>=0)
		return (*(tcp_ptr *)ptr).sock;	
	return -1;
}
int TCP::Send(int fd,char *data,int len=0)
{
	if(len==0)
		return send(fd, data, sizeof(data), 0);
	return send(fd, data, len, 0);
}
int TCP::Read(int fd,char *data,int len)
{
	return recv(fd, data, len, 0);	
}
int TCP::NoBlock(void)
{
	return fcntl((*(tcp_ptr *)ptr).sock, F_SETFL, fcntl((*(tcp_ptr *)ptr).sock, F_GETFL, 0)|O_NONBLOCK);	
}
int TCP::Block(void)
{
	return fcntl((*(tcp_ptr *)ptr).sock, F_SETFL, fcntl((*(tcp_ptr *)ptr).sock, F_GETFL, 0)& ~O_NONBLOCK);	
}
