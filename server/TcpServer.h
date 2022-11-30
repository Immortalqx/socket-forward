#ifndef SOCKET_FORWARD_TCPSERVER_H
#define SOCKET_FORWARD_TCPSERVER_H


#include "TcpSocket.h"

class TcpServer
{
public:
    TcpServer();

    ~TcpServer();

    int setListen(unsigned short port);

    TcpSocket *acceptConn(struct sockaddr_in *addr = nullptr);

private:
    int m_fd; // 监听的套接字
};


#endif //SOCKET_FORWARD_TCPSERVER_H
