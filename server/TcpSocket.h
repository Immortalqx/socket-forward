#ifndef SOCKET_FORWARD_TCPSOCKET_H
#define SOCKET_FORWARD_TCPSOCKET_H

#include <string>

class TcpSocket
{
public:
    TcpSocket();

    TcpSocket(int socket);

    ~TcpSocket();

    int connect_host(std::string ip, unsigned short port);

    //TODO: 需要修改成循环发送大文件的函数！
    int send_msg(const char *msg, int msg_len) const;

    //TODO: 需要求改成循环接收大文件的函数！
    char *recv_msg();

private:
    int m_fd; // 通信的套接字
    char buffer[BUFSIZ]; // 通信的缓存
};


#endif //SOCKET_FORWARD_TCPSOCKET_H
