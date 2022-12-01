#ifndef SOCKET_FORWARD_TCPSOCKET_H
#define SOCKET_FORWARD_TCPSOCKET_H

#include <string>

namespace SocketForward
{
    // 数据报
    typedef struct Datagram
    {
        // 数据的长度
        unsigned long length;
        // 保存的数据
        unsigned char buffer[BUFSIZ];
    } Datagram;

    // 长数据
    typedef struct Data
    {
        // 长度
        unsigned long length;
        // 内容
        unsigned char *buffer;
    } Data;

    class TcpSocket
    {
    public:
        TcpSocket();

        TcpSocket(int socket);

        ~TcpSocket();

        int connect_host(std::string ip, unsigned short port);

        void send_msg(void *msg, unsigned long msg_len) const;

        Data recv_msg() const;

    private:
        Data loop_recv(unsigned long loop_num) const;

    private:
        // 通信的套接字
        int m_fd;
    };

}
#endif //SOCKET_FORWARD_TCPSOCKET_H
