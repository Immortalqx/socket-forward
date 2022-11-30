cmake_minimum_required(VERSION 3.23)
project(socket_forward)

set(CMAKE_CXX_STANDARD 14)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

add_executable(socket_forward main.cpp TcpSocket.cpp TcpSocket.h TcpServer.cpp TcpServer.h ThreadPool.h)
target_link_libraries(socket_forward PRIVATE Threads::Threads)