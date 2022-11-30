#include <armadillo>
#include <arpa/inet.h>
#include <csignal>
#include "client.h"

client::client(const char *server_ip, int server_port)
{
    this->server_ip = server_ip;
    this->server_port = server_port;
}

void client::start()
{
    this->connect_();

    this->pid = fork();
    if (pid > 0)//父进程发送数据
    {
        this->send_();
    } else if (pid == 0)//子进程接收数据
    {
        this->recv_();
    } else
    {
        printf("Fail to call fork()\n");
        return;
    }

    /* 关闭套接字 */
    close(this->client_sockfd);
}

void client::connect_()
{
    /* 初始化目标服务器的网络信息 */
    memset(&this->remote_addr, 0, sizeof(this->remote_addr));           //数据初始化--清零
    this->remote_addr.sin_family = AF_INET;                       //设置为IP通信
    this->remote_addr.sin_addr.s_addr = inet_addr(this->server_ip);     //服务器IP地址
    this->remote_addr.sin_port = htons(this->server_port);              //服务器端口号

    /*创建客户端套接字--IPv4协议，面向连接通信，TCP协议*/
    if ((this->client_sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("socket error");
        return;
    }

    /*将套接字绑定到服务器的网络地址上*/
    if (connect(this->client_sockfd, (struct sockaddr *) &this->remote_addr, sizeof(struct sockaddr)) < 0)
    {
        perror("connect error");
        return;
    }
    printf("connected to server\n");
}

//TODO: 这个send需要修改成可以发布任意数据的函数！！！
void client::send_()
{
//    int test_len = 1;
    while (true)
    {
//        memset(this->buf_send, 0, sizeof(this->buf_send));
        scanf("%s", this->buf_send);
//        for (int i = 0; i < test_len; i++)
//        {
//            this->buf_send[i] = 'A';
//        }
//        this->buf_send[test_len] = '\0';

        if (!strcmp(this->buf_send, "quit"))
        {
            kill(this->pid, SIGSTOP);
            break;
        }

        send(this->client_sockfd, this->buf_send, strlen(this->buf_send), 0);

//        test_len += 2;
//        usleep(1000000);
    }
}

//TODO：这里的recv_也需要修改为可以发布任意数据的函数
//FIXME: 如果遇到接收数据量超过缓存的情况（需要多次接收的情况）
// 如何判断自己已经把数据接收完毕？接收完毕之后如何恢复成要使用的数据（比如图像或者音频）？
[[noreturn]] void client::recv_()
{
    while (true)
    {
        memset(this->buf_recv, 0, sizeof(this->buf_recv));
        if ((recv(this->client_sockfd, this->buf_recv, BUFSIZ, 0)) > 0)
        {
            printf("Recive from server: %s\n", this->buf_recv);
        }
    }
}
