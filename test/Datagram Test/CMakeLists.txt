# 声明要求的 cmake 最低版本
cmake_minimum_required(VERSION 3.24)

# 声明一个 cmake 工程
project(Demo)

# 设置C++编译器版本
set(CMAKE_CXX_STANDARD 17)

# 设置编译模式
set(CMAKE_BUILD_TYPE "Debug")

#添加OPENCV库
find_package(OpenCV REQUIRED)

#添加OpenCV头文件
include_directories(${OpenCV_INCLUDE_DIRS})

#显示OpenCV_INCLUDE_DIRS的值
message(${OpenCV_INCLUDE_DIRS})

# 添加一个可执行程序
add_executable(Demo main.cpp)

# 将库文件链接到可执行程序上
target_link_libraries(Demo ${OpenCV_LIBS})