#include <iostream>
#include <armadillo>
#include <opencv2/opencv.hpp>
#include "TcpSocket.h"

//FIXME: 这里的异常处理懒得写了。。。并且默认图像是CV_8UC3的格式

int to_int(SocketForward::Data data)
{
    char buffer[DATALEN];
    for (int i = 0; i < data.length; i++)
        buffer[i] = (char) data.buffer[i];
    buffer[data.length] = '\0';
    std::string result = buffer;
    return std::stoi(result);
}

[[noreturn]] void send_image(const SocketForward::TcpSocket &tcp)
{
    std::string image_path;
    while (true)
    {
        // 读取图像
        std::cin >> image_path;
        cv::Mat image = cv::imread(image_path);
        // 发送图像的row
        std::string row = std::to_string(image.rows);
        tcp.send_msg((void *) row.c_str(), row.size());// 话说这里为什么要强制类型转化，是不是有问题？
        usleep(5000);
        // 发送图像的col
        std::string col = std::to_string(image.cols);
        tcp.send_msg((void *) col.c_str(), col.size());// 话说这里为什么要强制类型转化，是不是有问题？
        usleep(5000);

        // PRINT TEST
        std::cout << "send image:\n" <<
                  "row:\t" << row << "\t col:\t" << col << std::endl;

        // 发送图像的data
        tcp.send_msg(image.data, image.rows * image.cols * 3);
        usleep(10000);

        // PRINT TEST
        std::cout << "send image finished" << std::endl;
    }
}

[[noreturn]] void recv_image(const SocketForward::TcpSocket &tcp)
{
    SocketForward::Data data;
    while (true)
    {
        // 接收图像的row
        data = tcp.recv_msg();
        int row = to_int(data);
        usleep(1000);
        // 接收图像的col
        data = tcp.recv_msg();
        int col = to_int(data);
        usleep(1000);
        // 接收图像的data
        data = tcp.recv_msg();
        usleep(1000);

        // PRINT TEST
        std::cout << "recv image:\n" <<
                  "row:\t" << row << "\t col:\t" << col << std::endl;
        
        // 转化成图像并且展示
        cv::Mat image = cv::Mat(row, col, CV_8UC3, data.buffer, 0);
        cv::imshow("recv image", image);
        cv::waitKey(5000);
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
        send_image(tcp);
    } else if (pid == 0)// 子进程接收数据
    {
        recv_image(tcp);
    } else
    {
        printf("Fail to call fork()\n");
        return -1;
    }
}