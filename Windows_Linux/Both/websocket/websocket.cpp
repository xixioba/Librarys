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
// #pragma comment(lib,"ws2_32.lib")
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

/*
1.握手。 
client第一次connet连接会发起握手协议，server在recv接收处解析，
判断如果是websocket的握手协议，那么同样组装好特定格式包头回复给client，建立连接。
*/
//判断是不是websocket协议
bool isWSHandShake(std::string &request)
{
    size_t i = request.find("GET");
    if(i == std::string::npos){
        return false;
    }
    return true;
}

//如果是，解析握手协议重新组装准备send回复给client
bool wsHandshake(std::string &request, std::string &response)
{
    //得到客户端请求信息的key
    std::string tempKey = request;
    size_t i = tempKey.find("Sec-WebSocket-Key");
    if(i == std::string::npos){
        return false;
    }
    tempKey = tempKey.substr(i + 19, 24);

    //拼接协议返回给客户端
    response = "HTTP/1.1 101 Switching Protocols\r\n";  
    response += "Connection: upgrade\r\n";  
    response += "Sec-WebSocket-Accept: ";  

    std::string realKey = tempKey;
    realKey += "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
    SHA1 sha;
    unsigned int message_digest[5];
    sha.Reset();
    sha << realKey.c_str();
    sha.Result(message_digest);
    for (int i = 0; i < 5; i++) {
        message_digest[i] = htonl(message_digest[i]);
    }
    realKey = base64_encode(reinterpret_cast<const unsigned char*>(message_digest), 20);
    realKey += "\r\n";
    response += realKey.c_str();
    response += "Upgrade: websocket\r\n\r\n";
    return true;
}


/*
2.接收client协议解析
首先解析包头信息
*/
bool wsReadHeader(char* cData, WebSocketStreamHeader* header)  
{  
    if (cData == NULL) return false;  

    const char *buf = cData;  
    header->fin = buf[0] & 0x80;  
    header->masked = buf[1] & 0x80;  
    unsigned char stream_size = buf[1] & 0x7F;  

    header->opcode = buf[0] & 0x0F;  
    // if (header->opcode == WS_FrameType::WS_CONTINUATION_FRAME) {  
    //     //连续帧  
    //     return false;  
    // }
    if (header->opcode == WS_TEXT_FRAME) {  
        //文本帧  
    }  
    else if (header->opcode == WS_BINARY_FRAME) {  
        //二进制帧
    }  
    else if (header->opcode == WS_CLOSING_FRAME) {  
        //连接关闭消息  
        return true;  
    }
    else if (header->opcode == WS_PING_FRAME) {  
        //  ping  
        return false;  
    }  
    else if (header->opcode == WS_PONG_FRAME) {  
        // pong  
        return false;  
    }  
    else {  
        //非法帧  
        return false;  
    }

    if (stream_size <= 125) {  
        //  small stream  
        header->header_size =6;  
        header->payload_size = stream_size;  
        header->mask_offset = 2;  
    }  
    else if (stream_size == 126) {  
        //  medium stream   
        header->header_size = 8;  
        unsigned short s = 0;  
        memcpy(&s, (const char*)&buf[2], 2);  
        header->payload_size = ntohs(s);  
        header->mask_offset = 4;  
    }  
    else if (stream_size == 127) {  

        unsigned long long l = 0;  
        memcpy(&l, (const char*)&buf[2], 8);  

        header->payload_size = l;  
        header->mask_offset = 10;  
    }  
    else {  
        //Couldnt decode stream size 非法大小数据包  
        return false;  
    }

    /*  if (header->payload_size > MAX_WEBSOCKET_BUFFER) {  
    return false;  
    } */ 

    return true;  
}

//然后根据包头解析出真实数据
bool wsDecodeFrame(WebSocketStreamHeader* header, char cbSrcData[], unsigned short wSrcLen, char cbTagData[])
{  
    const  char *final_buf = cbSrcData;  
    if (wSrcLen < header->header_size + 1) {  
        return false;
    }  

    char masks[4];  
    memcpy(masks, final_buf + header->mask_offset, 4);  
    memcpy(cbTagData, final_buf + header->mask_offset + 4, header->payload_size);  

    for (unsigned int i = 0; i < header->payload_size; ++i){  
        cbTagData[i] = (cbTagData[i] ^ masks[i % 4]);  
    }  
    //如果是文本包，在数据最后加一个结束字符“\0”
    if (header->opcode == WS_TEXT_FRAME)
        cbTagData[header->payload_size] = '\0';  

    return true;  
}  

//3.组装server发给client协议
bool wsEncodeFrame(std::string inMessage, std::string &outFrame, enum WS_FrameType frameType)  
{  
    const uint32_t messageLength = inMessage.size();  
    if (messageLength > 32767)  
    {  
        // 暂不支持这么长的数据  
        return false;
    }  

    uint8_t payloadFieldExtraBytes = (messageLength <= 0x7d) ? 0 : 2;
    // header: 2字节, mask位设置为0(不加密), 则后面的masking key无须填写, 省略4字节  
    uint8_t frameHeaderSize = 2 + payloadFieldExtraBytes;  
    uint8_t *frameHeader = new uint8_t[frameHeaderSize];
    memset(frameHeader, 0, frameHeaderSize);  

    // fin位为1, 扩展位为0, 操作位为frameType  
    frameHeader[0] = static_cast<uint8_t>(0x80 | frameType);  

    // 填充数据长度
    if (messageLength <= 0x7d)  
    {  
        frameHeader[1] = static_cast<uint8_t>(messageLength);  
    }  
    else  
    {  
        frameHeader[1] = 0x7e;
        uint16_t len = htons(messageLength);
        memcpy(&frameHeader[2], &len, payloadFieldExtraBytes);
    }  

    // 填充数据  
    uint32_t frameSize = frameHeaderSize + messageLength;
    char *frame = new char[frameSize + 1];
    memcpy(frame, frameHeader, frameHeaderSize);  
    memcpy(frame + frameHeaderSize, inMessage.c_str(), messageLength);  
    outFrame = std::string(frame, frameSize);
    delete[] frame;
    delete[] frameHeader;
    return true;
}

int main(int argc, char const *argv[])
{
    int len;
    char *buff=new char[1024];
    char *sbuff=new char[1024];
    std::string strout;
    TCP tcp(5001);
    
    while(1)
    {
        int fd=tcp.Accept();
        while(fd)
        {
            cout<<"Accept one"<<endl;
            len=tcp.Read(fd,buff,1024);
            if(len>0)
            {
                std::string str = buff;
                if(isWSHandShake(str)==true)
                {
                    wsHandshake(str,strout);
                    strcpy(sbuff, strout.c_str());
                    tcp.Send(fd,(char *)sbuff,strlen(sbuff));
                }
                else
                    continue;
            }
            cout<<"connect success"<<endl;
            while(fd)
            {
                len=tcp.Read(fd,buff,1024);
                if(len>0)
                {
                    WebSocketStreamHeader header;
                    wsReadHeader(buff,&header);
                    wsDecodeFrame(&header,buff,len,sbuff);
                    cout<<sbuff<<" "<<len<<endl;
                }
                // cout<<"send msg"<<endl;
                string msg="hello\n";
                wsEncodeFrame(msg,strout,WS_TEXT_FRAME);
                strcpy(sbuff, strout.c_str());
                tcp.Send(fd,(char *)sbuff,strlen(sbuff));
                sleep(1);
            }
        }       
        // close(fd);
    }
	return 0;
}