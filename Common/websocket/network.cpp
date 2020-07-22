#include "network.h"

struct udp_ptr
{
    struct sockaddr_in udp;
    int sock;
    int PORT;
    char *IP;
};

UDP::UDP(int NonBlocking)
{
    signal(SIGPIPE, SIG_IGN);
    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, SIGPIPE);
    sigprocmask(SIG_BLOCK, &set, NULL);
    this->NonBlocking = NonBlocking;
#ifdef _WIN32
    char opt = 1;
    WORD wVersionRequested;
    WSADATA wsaData;
    wVersionRequested = MAKEWORD(1, 1);
    if (WSAStartup(wVersionRequested, &wsaData) != 0)
    {
        printf("load dll error!\n");
        return;
    }
    memset(&addr, 0, sizeof(addr));
    socketfd = socket(PF_INET, SOCK_DGRAM, 0);
    setsockopt(socketfd, SOL_SOCKET, SO_REUSEADDR | SO_BROADCAST, &opt, sizeof(char));
    if (NonBlocking == 1)
    {
        unsigned long ul = 1;
        if (ioctlsocket(socketfd, FIONBIO, (unsigned long *)&ul) == SOCKET_ERROR)
            std::cout << "UDP set NonBlocking error!" << std::endl;
    }
#elif defined __linux__ || defined __APPLE__
    int opt = 1;
    bzero(&addr, sizeof(addr));
    socketfd = socket(PF_INET, SOCK_DGRAM, 0);
    setsockopt(socketfd, SOL_SOCKET, SO_REUSEADDR | SO_BROADCAST, &opt, sizeof(int));
    if (NonBlocking == 1)
    {
        int flags = fcntl(socketfd, F_GETFL, 0);
        fcntl(socketfd, F_SETFL, flags | O_NONBLOCK);
    }
    // struct timeval timeout = {60, 0};
    // setsockopt(socketfd, SOL_SOCKET, SO_SNDTIMEO, (char *)&timeout, sizeof(struct timeval));
    // setsockopt(socketfd, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(struct timeval));
#endif
    addr.sin_family = AF_INET;
}
UDP::~UDP(void)
{
#if defined __linux__ || defined __APPLE__
    close(socketfd);
#else
    closesocket(socketfd);
#endif
#ifdef _WIN32
//    WSACleanup();
#endif
}
int UDP::Bind(int port)
{
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    return ::bind(socketfd, (struct sockaddr *)&addr, sizeof(addr));
}
int UDP::Send(char *data, int len, char *ip, int port)
{
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(ip);
    return sendto(socketfd, data, len, 0, (struct sockaddr *)&addr, sizeof(addr));
}
int UDP::Read(char *data, int len)
{
#ifdef _WIN32
    int length = sizeof(sockaddr);
#elif defined __linux__ || defined __APPLE__
    socklen_t length = sizeof(sockaddr);
#endif
    return recvfrom(socketfd, data, len, 0, (struct sockaddr *)&addr, &length);
}

int UDP::Close(int socketfd)
{
#if defined __linux__ || defined __APPLE__
    return close(socketfd);
#else
    return closesocket(socketfd);
#endif
#ifdef _WIN32
//    WSACleanup();
#endif
}

TCP::TCP(int NonBlocking)
{
    signal(SIGPIPE, SIG_IGN);
    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, SIGPIPE);
    sigprocmask(SIG_BLOCK, &set, NULL);
    this->NonBlocking = NonBlocking;
#ifdef _WIN32
    char opt = 1;
    WORD wVersionRequested;
    WSADATA wsaData;
    wVersionRequested = MAKEWORD(1, 1);
    if (WSAStartup(wVersionRequested, &wsaData) != 0)
    {
        printf("load dll error!\n");
        return;
    }
    memset(&addr, 0, sizeof(addr));
    socketfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    setsockopt(socketfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(char));
    if (NonBlocking == 1)
    {
        unsigned long ul = 1;
        if (ioctlsocket(socketfd, FIONBIO, (unsigned long *)&ul) == SOCKET_ERROR)
            std::cout << "TCP set NonBlocking error!" << std::endl;
    }
#elif defined __linux__ || defined __APPLE__
    int opt = 1;
    bzero(&addr, sizeof(addr));
    socketfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    setsockopt(socketfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(int));
    if (NonBlocking == 1)
    {
        int flags = fcntl(socketfd, F_GETFL, 0);
        fcntl(socketfd, F_SETFL, flags | O_NONBLOCK);
    }
    // struct timeval timeout = {60, 0};
    // setsockopt(socketfd, SOL_SOCKET, SO_SNDTIMEO, (char *)&timeout, sizeof(struct timeval));
    // setsockopt(socketfd, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(struct timeval));
#endif
    addr.sin_family = AF_INET;
}
TCP::~TCP(void)
{
#if defined __linux__ || defined __APPLE__
    close(socketfd);
#else
    closesocket(socketfd);
#endif

#ifdef _WIN32
//    WSACleanup();
#endif
}
int TCP::Bind(int port)
{
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    return ::bind(socketfd, (struct sockaddr *)&addr, sizeof(addr));
}
int TCP::Listen(int max)
{
    return listen(socketfd, max);
}
int TCP::Accept()
{
    struct sockaddr_in client;
#ifdef _WIN32
    int length = sizeof(sockaddr);
#elif defined __linux__ || defined __APPLE__
    socklen_t length = sizeof(sockaddr);
#endif
    return accept(socketfd, (struct sockaddr *)&client, &length);
}
int TCP::Connect(int port = 0, char *ip = 0)
{
    if (port != 0)
        addr.sin_port = htons(port);
    if (ip != 0)
        addr.sin_addr.s_addr = inet_addr(ip);
    if (NonBlocking == 1)
    {
        int ret;
        ret = connect(socketfd, (struct sockaddr *)&addr, sizeof(addr));
        if (ret == 0)
        {
            std::cout << "tcp has connected\n"
                      << std::endl;
        }
        else if (ret < 0)
        {
            if (errno == EINPROGRESS)
            {
                fd_set writefds;
                FD_ZERO(&writefds);
                FD_SET(socketfd, &writefds);
                struct timeval timeout;
                timeout.tv_sec = 6;
                timeout.tv_usec = 0;
                ret = select(socketfd + 1, NULL, &writefds, NULL, &timeout);
                if (ret < 0)
                {
                    return -1;
                }
                if (ret == 0)
                {
                    std::cout << "connection timeout" << std::endl;
                    return -1;
                }
                else
                {
                    if (!FD_ISSET(socketfd, &writefds))
                    {
                        std::cout << "err, no events found!" << std::endl;
                        return -1;
                    }
                    else
                    {
                        int err = 0;
#ifdef _WIN32
                        int elen = sizeof(err);
#elif defined __linux__ || defined __APPLE__
                        socklen_t elen = sizeof(err);
#endif
                        ret = getsockopt(socketfd, SOL_SOCKET, SO_ERROR, (char *)&err, &elen);
                        if (ret < 0)
                        {
                            std::cout << "getsockopt" << std::endl;
                            return -1;
                        }
                        if (err != 0)
                        {
                            std::cout << "connect failed with the error: " << err << strerror(err) << std::endl;
                            return -1;
                        }
                        else
                        {
                            //                            std::cout<<"connected"<<std::endl;
                        }
                    }
                }
            }
        }
        return socketfd;
    }
    else
    {
        if (connect(socketfd, (struct sockaddr *)&addr, sizeof(addr)) >= 0)
            return socketfd;
    }
    return -1;
}
int TCP::Send(int fd, char *data, int len)
{
    return send(fd, data, len, 0);
}
int TCP::Read(int fd, char *data, int len)
{
    return recv(fd, data, len, 0);
}
int TCP::Close(int socketfd)
{
#if defined __linux__ || defined __APPLE__
    return close(socketfd);
#else
    return closesocket(socketfd);
#endif
#ifdef _WIN32
//    WSACleanup();
#endif
}

#if defined __linux__ || defined __APPLE__
bool TCP::IsAlive(int clientSocket)
{
    char buff[32];
    int recvBytes = recv(clientSocket, buff, sizeof(buff), MSG_PEEK);
    int sockErr = errno;
    //cout << "In close function, recv " << recvBytes << " bytes, err " << sockErr << endl;
    if (recvBytes > 0) //Get data
        return false;
    if ((recvBytes == -1) && (sockErr == EWOULDBLOCK)) //No receive data
        return false;
    return true;
}
#else
bool TCP::IsAlive(SOCKET clientSocket)
{
    bool ret = false;
    HANDLE closeEvent = WSACreateEvent();
    WSAEventSelect(clientSocket, closeEvent, FD_CLOSE);

    DWORD dwRet = WaitForSingleObject(closeEvent, 0);

    if (dwRet == WSA_WAIT_EVENT_0)
        ret = true;
    else if (dwRet == WSA_WAIT_TIMEOUT)
        ret = false;

    WSACloseEvent(closeEvent);
    return ret;
}
#endif
