cmake_minimum_required(VERSION 3.23)
project(client)

set(CMAKE_CXX_STANDARD 11)
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})
message(${OpenCV_INCLUDE_DIRS})

add_executable(client main.cpp TcpSocket.cpp TcpSocket.h)
target_link_libraries(client PRIVATE Threads::Threads ${OpenCV_LIBS})
