#ifndef __network
#define __network
//windows:add -lwsock32 for opject
class UDP
{
	void *ptr;
public:
	UDP(int port);//para port:No Specify--Client or Specify--Server(auto bind)
	~UDP(void);
	int Bind(int port);
	int Send(char *data,int len,char * ip,int port);
	int Read(char *data,int len);
	int NoBlock(void);
	int Block(void);
};

class TCP
{
	void *ptr;
public:
	TCP(int port,int max);//para port:No Specify--Client or Specify--Server(auto bind and listen);max:default=10
	~TCP(void);
	int Bind(int port);
	int Listen(int max);
	int Accept(void);
	int Connect(char *ip,int port);
	int Send(int fd,char *data,int len);
	int Read(int fd,char *data,int len);
	int NoBlock(void);
	int Block(void);
};
#endif