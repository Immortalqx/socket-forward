#include <iostream>
#include <cstring>
#include <cmath>

#include <opencv2/opencv.hpp>

#define DATALEN 3

// 数据报
typedef struct Datagram
{
    // 是否为最后一个数据报
    bool is_end;
    // 数据的长度
    unsigned long length;
    // 保存的数据
    unsigned char buffer[DATALEN];
} Datagram;

// 数据报数组
typedef struct DatagramArray
{
    // 数据报长度
    int length;
    // 数据报数组
    Datagram *datagram;
} DatagramArray;

// 输出数据报数组
void print_datagram(DatagramArray datagramArray, bool show_buffer = true)
{
    for (int i = 0; i < datagramArray.length; i++)
    {
        std::cout << "Datagram: " << i << std::endl <<
                  "is_end:\t" << std::boolalpha << datagramArray.datagram[i].is_end << std::endl;
        if (show_buffer)
        {
            std::cout << "buffer:\t";
            for (int j = 0; j < datagramArray.datagram[i].length; j++)
                std::cout << datagramArray.datagram[i].buffer[j];
            std::cout << std::endl;
        }
    }
    std::cout << std::endl;
}

// 循环发布测试
// TEST: 把data存入Datagram数组中
// NOTE: 在实际情况下，send是一个一个发送数据报，也知道数据的长度，所以输出数据报数组即可！
// FIXME: 由于unsigned char* 数组长度实在太难获取，只能通过传递参数的办法输入了！
DatagramArray send_test(void *data, unsigned long length)
{
    // 已经处理的数据长度
    unsigned long current_len = 0;
    // 数据报的长度
    int data_len = ceil((double) length / DATALEN);
    // 数据报数组
    auto datagram = new Datagram[data_len];

    // 处理到每一个数据报中
    for (int i = 0; i < data_len; i++)
    {
        // 判断是否为最后一个数据报
        if ((data_len - i) == 1)
            datagram[i].is_end = true;
        else
            datagram[i].is_end = false;
        // 打包数据
        datagram[i].length = std::min((unsigned long) DATALEN, length - current_len);
        for (unsigned long j = 0; j < datagram[i].length; j++)
            datagram[i].buffer[j] = ((unsigned char *) data)[current_len + j];
        current_len += datagram[i].length;
    }
    // 发布数据
    DatagramArray array;
    array.length = data_len;
    array.datagram = datagram;
    return array;
}

// 循环读取测试
// TEST: 从datagram数组中读取出char数组
// NOTE: recv是一个一个接收数据报，这里要模拟不知道最终数据长度的情况！
void *recv_test(DatagramArray array)
{
    // 最终接收的数据
    unsigned char *result = nullptr;
    // 接收的数据长度
    unsigned long result_len = 0;
    // 目前已经处理的长度
    unsigned long current_len = 0;
    // 开始循环接收数据！
    for (int num = 0; num < array.length; num++)
    {
        // 接收到一个数据报
        Datagram datagram = array.datagram[num];
        // 计算长度
        result_len += datagram.length;
        // 实时扩容
        result = (unsigned char *) realloc(result, result_len);
        // 进行数据报处理
        for (unsigned long i = 0; i < datagram.length; i++)
        {
            result[current_len++] = datagram.buffer[i];
        }
    }
    //FIXME: 如果传输字符串，这里有必要添加一个0；但如果是传输图像呢，会有生么影响？
    result[current_len] = '\0';
    return result;
}

void str_test()
{
    char buffer[BUFSIZ];

    std::cout << "输入一段字符串开始测试：" << std::endl;
    std::cin >> buffer;

    DatagramArray array = send_test(buffer, strlen(buffer));

    //print_datagram(array);

    auto result = (char *) recv_test(array);
    std::cout << result << std::endl;
}

void image_test()
{
    cv::Mat image = cv::imread("../image.jpg");
    DatagramArray array = send_test(image.data, image.cols * image.rows * 3);

    //print_datagram(array, false);

    auto data = (unsigned char *) recv_test(array);

    cv::Mat result = cv::Mat(image.rows, image.cols, CV_8UC3, data, 0);
    cv::imshow("input", image);
    cv::imshow("result", result);
    cv::waitKey(0);
}

void stoi_test()
{
    char buffer[BUFSIZ];
    std::cin >> buffer;

    // 数据报的长度
    int data_len = ceil((double) strlen(buffer) / DATALEN);
    // 数据报文的数目
    std::string top_len = std::to_string(data_len);
    // 输出最初的字符串
    std::cout << top_len.c_str() << "\t" << top_len.size() << std::endl;
    // 模拟被接收到
    memcpy(buffer, top_len.c_str(), top_len.size());
    buffer[top_len.size()] = '\0';//这一步非常重要！
    std::cout << buffer << std::endl;
    // 模拟处理成int
    std::string loop_num = buffer;
    std::cout << loop_num << "\t" << std::stoi(loop_num) << std::endl;
}

int main()
{
//    std::cout << "================string test================\n";
//    str_test();
//    std::cout << "================image test================\n";
//    image_test();
    while (true)
        stoi_test();
    return 0;
}
