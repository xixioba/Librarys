#include "network.h"

struct udp_ptr
{
	struct sockaddr_in udp;
	int sock;
	int PORT;
	char *IP;	
};

UDP::UDP(void)
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
}
UDP::~UDP(void)
{
	close((*(udp_ptr *)ptr).sock);
	delete (udp_ptr *)ptr;
}
int UDP::Bind(int port)
{
	(*(udp_ptr *)ptr).PORT=port;
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

TCP::TCP(void)
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
}
TCP::~TCP(void)
{
    close((*(tcp_ptr *)ptr).sock);
	delete (tcp_ptr *)ptr;
}
int TCP::Bind(int port)
{
	(*(tcp_ptr *)ptr).PORT=port;
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
