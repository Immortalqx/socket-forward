#include <iostream>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <armadillo>

#include "TcpServer.h"
#include "ThreadPool.h"

//最大客户端数量
#define MAXNUM 10

using namespace std;

struct SockInfo
{
    TcpSocket *tcp;
    struct sockaddr_in addr;
};

std::mutex info_array_mutex;
int current_num = 0;
struct SockInfo *info_array[MAXNUM];

void publish(const char *data, int data_len, SockInfo *except_info)
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

void *working(void *arg)
{
    SockInfo *pinfo = static_cast<struct SockInfo *>(arg);

    // 连接建立成功, 打印客户端的IP和端口信息
    char ip[32];
    printf("接入客户端，IP: %s, 端口: %d\n",
           inet_ntop(AF_INET, &pinfo->addr.sin_addr.s_addr, ip, sizeof(ip)),
           ntohs(pinfo->addr.sin_port));

    // 5. 通信
    while (true)
    {
        string msg = pinfo->tcp->recv_msg();
        if (!msg.empty()) cout << msg << endl << endl;
        else break;
        //广播数据
        publish(msg.c_str(), msg.length(), pinfo);
    }
    delete pinfo->tcp;
    delete pinfo;
    return nullptr;
}

int main()
{
    // 0. 创建线程池
    ThreadPool *thread_pool = new ThreadPool(MAXNUM);
    // 1. 创建监听的套接字
    TcpServer server;
    // 2. 绑定本地的IP port并设置监听
    server.setListen(10000);
    // 3. 阻塞并等待客户端的连接
    while (true)
    {
        SockInfo *info = new SockInfo;
        TcpSocket *tcp = server.acceptConn(&info->addr);
        if (tcp == nullptr)
        {
            cout << "重试...." << endl;
            usleep(1);
            continue;
        }
        if (current_num > 9)
        {
            cout << "到达最大连接数量...." << endl;
            usleep(10);
            continue;
        }
        // 4. 创建子线程处理数据
        info->tcp = tcp;
        thread_pool->enqueue(working, info);
        // 5. 更新客户端数组
        // TODO: 这里是否真的需要加锁？其他地方应该是不会修改这个东西的
        //  不过后面肯定要判断客户端的退出，到时候必然要修改
        info_array_mutex.lock();
        info_array[current_num++] = info;
        info_array_mutex.unlock();
    }
}