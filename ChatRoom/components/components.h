#ifndef COMPONENTS_H
#define COMPONENTS_H

#include <list>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <stdio.h>
#include <iostream>
#include <string.h>
#include <unistd.h>
#include <string>
#include <stdlib.h>

#define IP_addr "127.0.0.1"
#define Port 8888
#define MAX_EVENT_NUMBER 10000
#define MAX_MSG_SIZE 4096
#define EXIT_MSG "EXIT"

// save the Clients' sockfd
std::list<int> clients_list;

/**
 * @brief Set the Non Blocking object
 *
 * @param fd sock descriptor
 */
void setNonBlocking(int fd);

/**
 * @brief 添加sockfd到events数组中
 *
 * @param epollfd epoll fd
 * @param fd sock descriptor
 * @param enable_et true for use et, false not
 */
void addfd(int epollfd, int fd, bool enable_et);

/**
 * @brief 将fd从epoll中移除
 *
 * @param epollfd
 * @param fd sock descriptor
 */
void removefd(int epollfd, int fd);

/**
 * @brief 广播消息
 *
 * @param sockfd sock descriptor
 * @return int 返回字节数
 */
int sendBroadcastmessage(int sockfd);

void setNonBlocking(int fd)
{
    int old_flag = fcntl(fd, F_GETFL);
    int new_flag = old_flag | O_NONBLOCK;
    fcntl(fd, F_SETFL, new_flag);
}

void addfd(int epollfd, int fd, bool enable_et)
{
    epoll_event ev;
    ev.data.fd = fd;
    ev.events = enable_et ? (EPOLLIN | EPOLLET) : EPOLLIN;
    epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &ev);
    // 设置fd非阻塞模式
    setNonBlocking(fd);
}

void removefd(int epollfd, int fd)
{
    epoll_ctl(epollfd, EPOLL_CTL_DEL, fd, 0);
}

int sendBroadcastmessage(int sockfd)
{
    char buf[MAX_MSG_SIZE], msg[MAX_MSG_SIZE];
    bzero(&buf, MAX_MSG_SIZE);
    bzero(&msg, MAX_MSG_SIZE);

    printf("从客户端(clientfd = %d)收到消息\n", sockfd);
    int len = recv(sockfd, buf, MAX_MSG_SIZE, 0);
    // std::cout << "sendB len" << len << std::endl;
    if (len <= 0)
    {
        clients_list.remove(sockfd);
        printf("clinetID = %d 关闭了，现在有%ld个主机正在连接", sockfd, clients_list.size());
        return len;
    }
    std::string broadcat_msg = "ClientID = " + std::to_string(sockfd) + " says: " + std::string(buf) + "\n";
    sprintf(msg, broadcat_msg.c_str(), broadcat_msg.size());
    // 广播数据
    for (int itfd : clients_list)
    {
        if (itfd != sockfd)
        {
            if (send(itfd, msg, strlen(msg) + 1, 0) == -1)
            {
                perror("broadcast error");
                exit(-1);
            }
        }
    }
    return len;
}
#endif