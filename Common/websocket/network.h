#ifndef __network
#define __network
//windows:add -lwsock32 for opject
class UDP
{
	void *ptr;
public:
	UDP(int port=0);//para port:No Specify--Client or Specify--Server(auto bind)
	~UDP(void);
	int Bind(int port);//para port:auto bind the Specifed port
	int Send(char *data,int len,char * ip,int port);
	int Read(char *data,int len);
};

class TCP
{
	void *ptr;
public:
        TCP(int port=0,int listen_max=10);//para port:No Specify--Client or Specify--Server(auto bind and listen)
	~TCP(void);
	int Bind(int port);//开启服务端，指定端口
	int Listen(int max);
	int Accept();
	int Connect(int port,char *ip);
	int Send(int fd,char *data,int len);
	int Read(int fd,char *data,int len);
};
#endif
