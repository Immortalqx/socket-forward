#ifndef CLIENT_CLIENT_H
#define CLIENT_CLIENT_H

#include <cstdio>
#include <netinet/in.h>

class client
{
public:
    client(const char *server_ip, int server_port);

    void start();

private:
    void connect_();

    void send_();

    [[noreturn]] void recv_();

private:
    const char *server_ip;
    int server_port;

    int pid;

    int client_sockfd;
    char buf_recv[BUFSIZ];
    char buf_send[BUFSIZ];
    struct sockaddr_in remote_addr;
};


#endif //CLIENT_CLIENT_H
