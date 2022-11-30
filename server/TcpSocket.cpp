#include <sys/socket.h>
#include<arpa/inet.h>
#include <armadillo>
#include <netinet/in.h>

#include "TcpSocket.h"

TcpSocket::TcpSocket()
{
    m_fd = socket(AF_INET, SOCK_STREAM, 0);
}

TcpSocket::TcpSocket(int socket)
{
    m_fd = socket;
}

TcpSocket::~TcpSocket()
{
    if (m_fd > 0)
    {
        close(m_fd);
    }
}

int TcpSocket::connect_host(std::string ip, unsigned short port)
{
    // 连接服务器IP port
    struct sockaddr_in saddr{};
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(port);
    inet_pton(AF_INET, ip.data(), &saddr.sin_addr.s_addr);
    int ret = connect(m_fd, (struct sockaddr *) &saddr, sizeof(saddr));
    if (ret == -1)
    {
        perror("connect");
        return -1;
    }
    std::cout << "成功和服务器建立连接..." << std::endl;
    return ret;
}

int TcpSocket::send_msg(const char *msg, int msg_len) const
{
    return send(m_fd, msg, msg_len, 0);
}

char *TcpSocket::recv_msg()
{
    //接收信息
    printf("Receiving messages from client %d ...\n", m_fd);

    //NOTE: 此处的buffer不可以是一个指针，必须是一个数组
    int buffer_length = recv(m_fd, buffer, BUFSIZ, 0);

    if (buffer_length < 0)
        return nullptr;

    buffer[buffer_length] = '\0';
    return buffer;
}