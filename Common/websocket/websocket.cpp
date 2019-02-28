#include "websocket.h"

/*
1.握手。 
client第一次connet连接会发起握手协议，server在recv接收处解析，
判断如果是websocket的握手协议，那么同样组装好特定格式包头回复给client，建立连接。
*/
//判断是不是websocket协议
static bool isWSHandShake(std::string &request)
{
    size_t i = request.find("GET");
    if(i == std::string::npos){
        return false;
    }
    return true;
}

//如果是，解析握手协议重新组装准备send回复给client
static bool wsHandshake(std::string &request, std::string &response)
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
static bool wsReadHeader(char* cData, WebSocketStreamHeader* header)  
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
static bool wsDecodeFrame(WebSocketStreamHeader* header, char cbSrcData[], unsigned short wSrcLen, char cbTagData[])
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
static bool wsEncodeFrame(std::string inMessage, std::string &outFrame, enum WS_FrameType frameType)  
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



static char* wsEncodeFrameBytes(char* inMessage,enum WS_FrameType frameType,uint32_t *length)  
{  
    uint32_t messageLength;
    if(*length==0)
        messageLength = strlen(inMessage)+1;
    else
        messageLength = *length;
    if (messageLength > 32767)  
    {  
        // 暂不支持这么长的数据  
        return NULL;
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
    memcpy(frame + frameHeaderSize, inMessage, messageLength);
    *length=frameSize;
    delete[] frameHeader;
    return frame;
}

int WEBSOCKET::Send(int fd,char *data,uint32_t len)
{
    uint32_t length;
    char *psend;
    if(fd>0)
    {
        if(len==0)
        {
            length=strlen(data);
            psend=wsEncodeFrameBytes(data,WS_TEXT_FRAME,&length);
        }
        else
        {
            length=len;
            psend=wsEncodeFrameBytes(data,WS_BINARY_FRAME,&length);
        }
        TCP::Send(fd, psend, length);
        delete psend;
        return 0;
    }
    return -1;
}

int WEBSOCKET::Read(int fd,char *data,uint32_t len)
{
    if(fd>0)
    {
        char *buff=new char[len];
        len=TCP::Read(fd,buff,len);
        if(len>0)
        {
            WebSocketStreamHeader header;
            wsReadHeader(buff,&header);
            wsDecodeFrame(&header,buff,len,data);
        }
        delete buff;
        return 0;   
    }
    return -1;
}

int Thread::start()
{
    if(pthread_create(&pid,NULL,start_thread,(void *)this) != 0) //´创建一个线程(必须是全局函数)
    {    
        return -1; 
    }    
    return 0;
}

void* Thread::start_thread(void *arg) //静态成员函数只能访问静态变量或静态函数，通过传递this指针进行调用
{
    Thread *ptr = (Thread *)arg;
    ptr->run();  //线程的实体是run
}

void WEBSOCKET::run()
{
    int len;
    char *buff=new char[1024];
    char *sbuff=new char[1024];
    readbuff=new char[4096];
    std::string strout;
    while(1)
    {
        int fd=Accept();
        while(fd)
        {
            len=TCP::Read(fd,buff,1024);
            if(len>0)
            {
                std::string str = buff;
                if(isWSHandShake(str)==true)
                {
                    wsHandshake(str,strout);
                    TCP::Send(fd,(char *)strout.c_str(),strout.size());
                }
                else
                    continue;
            }
            cout<<"connect success"<<endl;
            while(fd)
            {
                webfd=fd;
                Read(fd,readbuff,4096);
            }
        }
    }  
}


// int main(int argc, char const *argv[])
// {
//     WEBSOCKET ws(5001);
//     ws.start();
//     char str1[]={1,2,3,0,5};
//     char *str2=(char *)"hello web!";
//     uint32_t length=5;
//     while(1)
//     {
//         if(ws.webfd>0)
//         {
//             ws.Send(ws.webfd,str2);
//         }
//         Delay(1);

//     }
// 	return 0;
// }