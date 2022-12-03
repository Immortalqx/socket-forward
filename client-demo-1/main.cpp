#include <iostream>
#include <armadillo>

#include "TcpSocket.h"

[[noreturn]] void send_string(const SocketForward::TcpSocket &tcp)
{
    char buffer[BUFSIZ];
    while (true)
    {
        std::cin >> buffer;
        tcp.send_msg(buffer, strlen(buffer));
        usleep(10000);
    }
}

[[noreturn]] void recv_string(const SocketForward::TcpSocket &tcp)
{
    SocketForward::Data data;
    while (true)
    {
        data = tcp.recv_msg();

        std::cout << "received:\t" << std::endl;
        for (int i = 0; i < data.length; i++)
        {
            std::cout << data.buffer[i];
        }
        std::cout << std::endl;
    }
}

int main()
{
    // 1. 创建通信的套接字
    SocketForward::TcpSocket tcp;

    // 2. 连接服务器IP port
    int ret = tcp.connect_host("127.0.0.1", 10000);
    if (ret < 0)
    {
        std::cout << "connected error" << std::endl;
        return -1;
    }
    std::cout << "connected to server" << std::endl;

    // 3. 通信
    int pid = fork();

    if (pid > 0)// 父进程发送数据
    {
        send_string(tcp);
    } else if (pid == 0)// 子进程接收数据
    {
        recv_string(tcp);
    } else
    {
        printf("Fail to call fork()\n");
        return -1;
    }
}