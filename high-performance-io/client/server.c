/*************************************************************************
        > Copyright(c)  NEWPLAN, all rights reserved.
        > File Name   : server.c
        > Author      : NEWPLAN
        > Mail        : newplan001@163.com
        > Created Time: 2018年11月06日 星期二 16时21分51秒
 ************************************************************************/

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

static void userHelp(const char *str)
{
    printf("%s [local_ip] [local_port]\n", str);
}

int startup(const char *_ip, int _port)
{
    //socket打开一个 网络通讯端口，其中AF_INET:表示IPV4，SOCK_STREAM：表示面向流的传输，
    //protocol参数默认选择为0
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1)
    {
        perror("socket()");
        exit(-1);
    }
    struct sockaddr_in local;
    local.sin_family = AF_INET;
    local.sin_port = htons(_port);
    local.sin_addr.s_addr = inet_addr(_ip);
    //bind的作用是将参数sock与local绑定在一起
    //使sock这个文件描述符监听local所描述的地址与端口号
    //成功返回0，失败返回-1
    if (bind(sock, (struct sockaddr *)&local, sizeof(local)) < 0) //success zero is return
    {
        perror("bind()");
        exit(-2);
    }
    //listen声明sock处于监听状态，并且最多允许5个客户端处于连接等待状态
    //如果收到更多的请求便忽略
    if (listen(sock, 5) == -1) //add listener
    {
        perror("listen()");
        exit(-3);
    }
    return sock;
}

void *handlerRequest(void *arg)
{
    int new_fd = *((int *)arg);
    printf("new_fd=%d\n", new_fd);
    while (1)
    {
        char buffer[1024];
        ssize_t s = read(new_fd, buffer, sizeof(buffer));
        if (s == -1)
        {
            perror("read");
        }
        if (s > 0)
        {

            buffer[s] = '\0';
            //printf("client:%s\n", buffer);
            //write(new_fd, buffer, strlen(buffer));
        }
        else
        {
            printf("read done...break\n");
            break;
        }
    }
}
int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        userHelp(argv[0]);
        return 1;
    }
    int listen_sock = startup(argv[1], atoi(argv[2]));
    while (1)
    {
        struct sockaddr_in client;
        socklen_t len = sizeof(client);
        //accept阻塞式等待，用来接收连接
        int new_fd = accept(listen_sock, (struct sockaddr *)&client, &len);
        if (new_fd < 0)
        {
            perror("accept()");
            continue;
        }
        printf("get a new client,%s:%d\n", inet_ntoa(client.sin_addr), ntohs(client.sin_port));
        pthread_t id;
        pthread_create(&id, NULL, handlerRequest, (void *)(&new_fd));
        pthread_detach(id);
        //return 0;
    }
    return 0;
}