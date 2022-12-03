#include <iostream>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <armadillo>

#include "TcpServer.h"
#include "ThreadPool.h"

// 最大客户端数量
#define MAXNUM 10

struct SockInfo
{
    SocketForward::TcpSocket *tcp;
    struct sockaddr_in addr;
};

std::mutex info_array_mutex;
int current_num = 0;
struct SockInfo *info_array[MAXNUM];

void publish(void *data, unsigned long data_len, SockInfo *except_info)
{
    info_array_mutex.lock();
    for (int i = 0; i < current_num; i++)
    {
        if (info_array[i] == except_info)
            continue;

        info_array[i]->tcp->send_msg(data, data_len);
    }
    info_array_mutex.unlock();
}

[[noreturn]] void *working(void *arg)
{
    auto pinfo = static_cast<struct SockInfo *>(arg);

    // 连接建立成功, 打印客户端的IP和端口信息
    char ip[32];
    printf("接入客户端，IP: %s, 端口: %d\n",
           inet_ntop(AF_INET, &pinfo->addr.sin_addr.s_addr, ip, sizeof(ip)),
           ntohs(pinfo->addr.sin_port));

    SocketForward::Data data;
    // 5. 通信
    while (true)
    {
        // 接收数据
        data = pinfo->tcp->recv_msg();
        // 广播数据
        publish(data.buffer, data.length, pinfo);
    }
//    delete pinfo->tcp;
//    delete pinfo;
//    return nullptr;
}

int main()
{
    // 0. 创建线程池
    auto thread_pool = new ThreadPool(MAXNUM);
    // 1. 创建监听的套接字
    TcpServer server;
    // 2. 绑定本地的IP port并设置监听
    server.setListen(10000);
    // 3. 阻塞并等待客户端的连接
    while (true)
    {
        auto info = new SockInfo;
        SocketForward::TcpSocket *tcp = server.acceptConn(&info->addr);
        if (tcp == nullptr)
        {
            std::cout << "重试...." << std::endl;
            sleep(1);
            continue;
        }
        if (current_num > 9)
        {
            std::cout << "到达最大连接数量...." << std::endl;
            sleep(10);
            continue;
        }
        // 4. 创建子线程处理数据
        info->tcp = tcp;
        thread_pool->enqueue(working, info);
        // 5. 更新客户端数组
        info_array_mutex.lock();
        info_array[current_num++] = info;
        info_array_mutex.unlock();
    }
}