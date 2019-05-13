#ifndef __WEBSOCKET_H
#define __WEBSOCKET_H 

#include "sha1.h"
#include "base64.h"
#include "network.h"
#include <vector>
#include <thread>

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

class WEBSOCKET:public TCP
{
    using TCP::TCP;
public:
    std::vector<int> webfds;
    WEBSOCKET(int port=0,int listen=10);
    ~WEBSOCKET();
    int Send(int fd,char *data,uint32_t len=0);//len==0->string len>0==>byte
	int Read(int fd,char *data,uint32_t len);
    void run();
};

#endif
