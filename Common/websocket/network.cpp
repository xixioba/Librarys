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



#include "network.h"


struct udp_ptr
{
	struct sockaddr_in udp;
	int sock;
	int PORT;
	char *IP;	
};

UDP::UDP(int port)
{
	ptr=new udp_ptr;
	(*(udp_ptr *)ptr).PORT=port;
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
	memset(&(*(udp_ptr *)ptr).udp,0,sizeof((*(udp_ptr *)ptr).udp));
	(*(udp_ptr *)ptr).sock=socket(PF_INET,SOCK_DGRAM,0);
	setsockopt((*(udp_ptr *)ptr).sock,SOL_SOCKET,SO_REUSEADDR|SO_BROADCAST,&opt,sizeof(char));
#elif __linux__
	int opt=1;
	bzero(&(*(udp_ptr *)ptr).udp,sizeof((*(udp_ptr *)ptr).udp));
	(*(udp_ptr *)ptr).sock=socket(PF_INET,SOCK_DGRAM,0);
	setsockopt((*(udp_ptr *)ptr).sock,SOL_SOCKET,SO_REUSEADDR|SO_BROADCAST,&opt,sizeof(int));
#endif
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
	if((*(udp_ptr *)ptr).PORT!=0)
		close((*(udp_ptr *)ptr).sock);
	(*(udp_ptr *)ptr).udp.sin_port=htons(port);
	(*(udp_ptr *)ptr).udp.sin_addr.s_addr=htonl(INADDR_ANY);
	return bind((*(udp_ptr *)ptr).sock,(struct sockaddr *)&(*(udp_ptr *)ptr).udp,sizeof((*(udp_ptr *)ptr).udp));
}
int UDP::Send(char *data,int len,char * ip=0,int port=0)
{
	if(port!=0)
		(*(udp_ptr *)ptr).udp.sin_port=htons(port);
	else
	{
		if((*(udp_ptr *)ptr).PORT==0)
			return -1;
	}
	if(ip!=0)
		(*(udp_ptr *)ptr).udp.sin_addr.s_addr=inet_addr(ip);
	else
	{
		if((*(udp_ptr *)ptr).IP==0)
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

struct tcp_ptr
{
	struct sockaddr_in tcp;
	int sock;
	int PORT;
	char *IP;
};

TCP::TCP(int port=0,int listen_max)
{
	ptr=new tcp_ptr;
	(*(tcp_ptr *)ptr).PORT=port;
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
	memset(&(*(tcp_ptr *)ptr).tcp,0,sizeof((*(tcp_ptr *)ptr).tcp));
	(*(tcp_ptr *)ptr).sock=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	setsockopt((*(tcp_ptr *)ptr).sock,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(char));
#elif __linux__
	int opt=1;
	bzero(&(*(tcp_ptr *)ptr).tcp,sizeof((*(tcp_ptr *)ptr).tcp));
	(*(tcp_ptr *)ptr).sock=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	setsockopt((*(tcp_ptr *)ptr).sock,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(int));
#endif
	(*(tcp_ptr *)ptr).tcp.sin_family=AF_INET;
	if(port>0 && listen_max>0)
	{
		Bind(port);
		Listen(listen_max);
	}
}
TCP::~TCP()
{
	close((*(tcp_ptr *)ptr).sock);
	delete (tcp_ptr *)ptr;
}
int TCP::Bind(int port)
{
	if((*(tcp_ptr *)ptr).PORT!=0)
		close((*(tcp_ptr *)ptr).sock);
	(*(tcp_ptr *)ptr).tcp.sin_port=htons(port);
	(*(tcp_ptr *)ptr).tcp.sin_addr.s_addr=htonl(INADDR_ANY);
	return bind((*(tcp_ptr *)ptr).sock,(struct sockaddr *)&(*(tcp_ptr *)ptr).tcp,sizeof((*(tcp_ptr *)ptr).tcp));
}
int TCP::Listen(int max)
{
	return listen((*(tcp_ptr *)ptr).sock,max);
}
int TCP::Accept()
{
	struct sockaddr_in client;
	#ifdef _WIN32
	int length=sizeof(sockaddr);
	#elif __linux__
	socklen_t length=sizeof(sockaddr);
	#endif
	return accept((*(tcp_ptr *)ptr).sock, (struct sockaddr*)&client, &length);
}
int TCP::Connect(int port=0,char *ip=0) 
{
	if(port!=0)
		(*(tcp_ptr *)ptr).tcp.sin_port=htons(port);
	else if((*(tcp_ptr *)ptr).PORT!=0)
	{
		(*(tcp_ptr *)ptr).tcp.sin_port=htons((*(tcp_ptr *)ptr).PORT);
	}
	else
		return -1;
	if(ip!=0)
		(*(tcp_ptr *)ptr).tcp.sin_addr.s_addr=inet_addr(ip);
	else if((*(tcp_ptr *)ptr).IP!=0)
	{
		(*(tcp_ptr *)ptr).tcp.sin_addr.s_addr=inet_addr((*(tcp_ptr *)ptr).IP);
	}
	else
		return -1;
	if(connect((*(tcp_ptr *)ptr).sock, (struct sockaddr*) &(*(tcp_ptr *)ptr).tcp, sizeof((*(tcp_ptr *)ptr).tcp))>=0)
		return (*(tcp_ptr *)ptr).sock;	
	return -1;
}
int TCP::Send(int fd,char *data,int len)
{
	return send(fd, data, len, 0);
}
int TCP::Read(int fd,char *data,int len)
{
	return recv(fd, data, len, 0);	
}

// int main(int argc, char const *argv[])
// {
// 	int len;
// 	char *buff=new char[100];
// 	TCP tcp(5001);
// 	TCP tcp2;
// 	// while(1)
// 	// {
// 	// 	int fd1=tcp2.Connect(5002,(char *)"127.0.0.1");
// 	// 	if(fd1>=0)
// 	// 	{
// 	// 		while(fd1)
// 	// 		{
// 	// 			tcp2.Send(fd1,(char *)"hello",5);
// 	// 			Delay(1);
// 	// 			len=tcp2.Read(fd1,buff,100);
// 	// 			if(len>0)
// 	// 			{
// 	// 				printf("%s\n",buff );
// 	// 			}
// 	// 		}
			
// 	// 	}
// 	// 	Delay(1);
// 	// }
// 	while(1)
// 	{
// 		int fd=tcp.Accept();
// 		while(fd)
// 		{	
// 			len=tcp.Read(fd,buff,100);
// 			if(len>0)
// 			{
// 				printf("%s\n",buff );
// 			}
// 			tcp.Send(fd,(char *)"hello",6);
// 		}		
// 		// close(fd);
// 	}

// 	return 0;
// }

// int main(int argc, char const *argv[])
// {
// 	UDP udp;
// 	UDP udp2;
// 	udp2.Bind(4001);
// 	int len;
// 	char *buff=new char[100];
// 	while (1)
// 	{
// 		udp.Send((char *)"hello 4002",(char *)"127.0.0.1",4002);		
// 		udp.Send((char *)"hello 4003",(char *)"127.0.0.1",4003);
// 		#ifdef _WIN32
// 		Sleep(1000);
// 		#elif __linux__
// 		sleep(1);
// 		#endif
// 		len=udp2.Read(buff,100);
// 		if(len>0)
// 			printf("%s %d\n",buff,len);
// 		else
// 			printf("no data\n");
// 	}
// 	return 0;
// }