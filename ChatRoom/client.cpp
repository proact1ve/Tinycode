#include "components/components.h"

int main(int argc, char **argv)
{
    // 创建sockfd
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    // bind
    sockaddr_in serveraddr;
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(Port);
    inet_pton(AF_INET, IP_addr, &serveraddr.sin_addr);

    if (connect(sockfd, (sockaddr *)&serveraddr, sizeof serveraddr) == -1)
    {
        perror("connect");
        exit(-1);
    }
    // 创建管道，其中fd[0]用于父进程，fd[1]用于紫禁城
    int pipe_fd[2];
    if (pipe(pipe_fd) < 0)
    {
        perror("pipe");
        exit(-1);
    }
    int epfd = epoll_create(5);
    epoll_event events[2];
    addfd(epfd, sockfd, true);
    addfd(epfd, pipe_fd[0], true);

    char msg[MAX_MSG_SIZE] = {0};

    int pid = fork();
    bool flag = true;
    if (pid < 0)
    {
        perror("fork error");
        exit(-1);
    }

    // 子进程
    else if (pid == 0)
    {
        //子进程负责写入管道，因此先关闭读端
        close(pipe_fd[0]);
        printf("输入 exit 来退出聊天室\n");

        while (flag)
        {
            bzero(&msg, MAX_MSG_SIZE);
            fgets(msg, MAX_MSG_SIZE, stdin);
            // printf("flag");
            if (strncasecmp(msg, EXIT_MSG, strlen(EXIT_MSG)) == 0)
            {
                flag = false;
            }
            // 子进程将信息输入管道
            else
            {
                if (write(pipe_fd[1], msg, strlen(msg) + 1) < 0)
                {
                    perror("write to pipe");
                    exit(-1);
                }
            }
        }
    }
    else //父进程
    {
        //父进程负责读管道数据，因此先关闭写端
        close(pipe_fd[1]);

        while (flag)
        {
            int epoll_num = epoll_wait(epfd, events, 2, -1);

            for (int i = 0; i < epoll_num; i++)
            {
                bzero(&msg, MAX_MSG_SIZE);
                int evfd = events[i].data.fd;
                if (evfd == sockfd)
                {
                    int ret = recv(sockfd, msg, MAX_MSG_SIZE, 0);
                    if (ret == 0)
                    {
                        std::cout << "服务器关闭，正在关闭。。。" << std::endl;
                        close(sockfd);
                        flag = false;
                    }
                    else
                        std::cout << msg << std::endl;
                }
                else
                {
                    //子进程写入事件发生，父进程处理并发送服务端
                    int ret = read(evfd, msg, MAX_MSG_SIZE);
                    if (ret == 0)
                        flag = false;
                    else
                    {
                        send(sockfd, msg, MAX_MSG_SIZE, 0);
                    }
                }
            }
        }
    }
    if (pid)
    {
        close(pipe_fd[0]);
        close(sockfd);
    }
    else
    {
        close(pipe_fd[1]);
    }
    return 0;
}