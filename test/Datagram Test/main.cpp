#include <iostream>
#include <cstring>
#include <cmath>

#include <opencv2/opencv.hpp>

#define DATALEN 1024

// 数据报
typedef struct Datagram
{
    // 是否为最后一个数据报
    bool is_end;
    // 保存的数据
    // NOTE: 这里+1是为了存放'\0'，测试发现如果不这么做，DATALEN取某些值时，recv接受的数据会混乱！
    // FIXME: 这里使用char会导致image图像传输受损（应该是溢出了），但是使用unsigned char也不能够完全解决问题！
    //  这里如何处理呢？如何把数据当成二进制来传输？？使用byte[]可以吗？？？
    char buffer[DATALEN + 1];
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
            std::cout << "buffer:\t" << datagramArray.datagram[i].buffer << std::endl;
    }
}

// 循环发布测试
// TEST: 把data存入Datagram数组中
// NOTE: 在实际情况下，send是一个一个发送数据报，也知道数据的长度，所以输出数据报数组即可！
DatagramArray send_test(void *data)
{
    // 数据总长度
    int length = strlen((char *) data);
    // 已经处理的数据长度
    int current_len = 0;
    // 数据报的长度
    int data_len = ceil(length * 1.0 / DATALEN);
    // 数据报数组
    Datagram *datagram = new Datagram[data_len];

    // 处理到每一个数据报中
    for (int i = 0; i < data_len; i++)
    {
        // 判断是否为最后一个数据报
        if ((data_len - i) == 1)
            datagram[i].is_end = true;
        else
            datagram[i].is_end = false;
        // 打包数据
        int j = 0;
        for (; j < std::min(DATALEN, length - current_len); j++)
            datagram[i].buffer[j] = ((char *) data)[current_len + j];
        datagram[i].buffer[j] = '\0';
        current_len += j;
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
    // 声明数组
    char *result = nullptr;
    int result_len = 0;

    // 目前已经处理的长度
    int current_len = 0;
    // 开始循环接收数据！
    for (int num = 0; num < array.length; num++)
    {
        // 接收到一个数据报
        Datagram datagram = array.datagram[num];

        // 考虑最后一个数据报长度不固定的情况
        if (datagram.is_end)
            result_len = result_len + strlen(datagram.buffer);
        else
            result_len += DATALEN;
        // 实时扩容
        result = (char *) realloc(result, result_len);
        // 进行数据报处理
        for (int i = 0; i < strlen(datagram.buffer); i++)
        {
            result[current_len++] = datagram.buffer[i];
        }
    }
    return result;
}

[[maybe_unused]] void realloc_test()
{
    // 如此声明的数组，是否可以被realloc？ 不可以！
    //char result[DATALEN];

    // 声明默认长度的数组
    int result_len = DATALEN;
    char *result = (char *) malloc(result_len);

    for (int i = 0; i < result_len - 1; i++)
        result[i] = 'W';
    result[result_len - 1] = '\0';

    std::cout << result_len << std::endl;
    std::cout << result << std::endl << std::endl;

    for (int t = 0; t < 3; t++)
    {
        result_len += DATALEN;
        result = (char *) realloc(result, result_len);

        // NOTE: 这里记得-1，不然不能够覆盖前面的终结符！
        for (int i = result_len - DATALEN - 1; i < result_len - 1; i++)
            result[i] = 'M';
        result[result_len - 1] = '\0';

        std::cout << result_len << std::endl;
        std::cout << result << std::endl << std::endl;
    }
}

void str_test()
{
    char buffer[BUFSIZ];

    std::cout << "输入一段字符串开始测试：" << std::endl;
    std::cin >> buffer;

    DatagramArray array = send_test(buffer);
    print_datagram(array);

    char *result = (char *) recv_test(array);
    std::cout << result << std::endl;
}

void image_test()
{
    cv::Mat image = cv::imread("../image.png");
    DatagramArray array = send_test(image.data);
    //print_datagram(array, false);

    char *data = (char *) recv_test(array);

    cv::Mat result = cv::Mat(image.rows, image.cols, CV_8UC3, data, 0);
    cv::imshow("result", result);
    cv::waitKey(0);
}

int main()
{
//    std::cout << "================realloc test================\n";
//    realloc_test();
//    std::cout << "================string test================\n";
//    str_test();
    std::cout << "================image test================\n";
    image_test();

    return 0;
}
