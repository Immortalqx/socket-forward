#include <sys/socket.h>
#include<arpa/inet.h>
#include <armadillo>
#include <netinet/in.h>

#include "TcpSocket.h"

namespace SocketForward
{
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
            perror("can't connect!");
            return -1;
        }
        std::cout << "connect success!" << std::endl;
        return ret;
    }

    void TcpSocket::send_msg(void *msg, unsigned long msg_len) const
    {
        // 已经处理的数据长度
        unsigned long current_len = 0;
        // 数据报的长度
        int data_len = ceil((double) msg_len / BUFSIZ);
        // 数据报数组
        auto datagram = new Datagram[data_len];

        // 处理到每一个数据报中
        for (int i = 0; i < data_len; i++)
        {
            // 打包数据
            datagram[i].length = std::min((unsigned long) BUFSIZ, msg_len - current_len);
            for (unsigned long j = 0; j < datagram[i].length; j++)
                datagram[i].buffer[j] = ((unsigned char *) msg)[current_len + j];
            current_len += datagram[i].length;
        }

        // 首先发送数据报文的数目
        std::string top_len = std::to_string(data_len);
        send(m_fd, top_len.c_str(), top_len.size(), 0);
        // 再逐一发送接下来的报文
        for (int i = 0; i < data_len; i++)
            send(m_fd, datagram[i].buffer, datagram[i].length, 0);
    }

    Data TcpSocket::recv_msg() const
    {
        //接收信息
        std::cout << "receive from client:" << m_fd << std::endl;
        //接收第一个消息，得到数据报文的数目
        char buffer[BUFSIZ];
        int len = recv(m_fd, buffer, BUFSIZ, 0);
        buffer[len] = '\0';
        std::string loop_num = buffer;
        //循环接收
        return loop_recv(std::stoi(loop_num));
    }

    Data TcpSocket::loop_recv(unsigned long loop_num) const
    {
        // 最终接收的数据
        unsigned char *result = nullptr;
        // 接收的数据长度
        unsigned long result_len = 0;
        // 目前已经处理的长度
        unsigned long current_len = 0;
        // 开始循环接收数据！
        for (int num = 0; num < loop_num; num++)
        {
            // 接收到一个数据报
            Datagram datagram;
            datagram.length = recv(m_fd, datagram.buffer, BUFSIZ, 0);
            // 计算长度
            result_len += datagram.length;
            // 实时扩容
            result = (unsigned char *) realloc(result, result_len);
            // 进行数据报处理
            for (unsigned long i = 0; i < datagram.length; i++)
                result[current_len++] = datagram.buffer[i];
        }
        //FIXME: 如果传输字符串，这里有必要添加一个0；但如果是传输图像呢，会有生么影响？
        result[current_len] = '\0';
        Data data;
        data.length = result_len;
        data.buffer = result;
        return data;
    }
}