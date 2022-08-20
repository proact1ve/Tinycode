#include "components/components.h"

int main()
{
    // 创建listenfd
    int listenFd = socket(AF_INET, SOCK_STREAM, 0);
    // 绑定端口IP
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(Port);
    addr.sin_addr.s_addr = INADDR_ANY;
    if (bind(listenFd, (sockaddr *)&addr, sizeof addr) == -1)
    {
        perror("bind");
        exit(-1);
    }

    // 监听
    listen(listenFd, 5);
    std::cout << "Listening to " << IP_addr << std::endl;

    // 创建Epoll对象，事件数组，并添加监听事件
    epoll_event events[MAX_EVENT_NUMBER];
    int epfd = epoll_create(10);
    // 添加fd
    addfd(epfd, listenFd, true);

    while (true)
    {
        std::cout << "epoll_wait" << std::endl;
        int ev_num = epoll_wait(epfd, events, MAX_EVENT_NUMBER, -1);
        if ((ev_num < 0) && errno != EINTR)
        {
            std::cout << "epoll Failure" << std::endl;
            break;
        }
        for (int i = 0; i < ev_num; i++)
        {
            int sockfd = events[i].data.fd;
            // std::cout << sockfd << std::endl;
            // 假如说有新的主机连接进来
            if (sockfd == listenFd)
            {
                struct sockaddr_in client_addr;
                socklen_t client_addr_len = sizeof client_addr;
                int clientfd = accept(listenFd, (sockaddr *)&client_addr, &client_addr_len);
                std::cout << "New client connected : IP: " << inet_ntoa(client_addr.sin_addr)
                          << " PORT: " << ntohs(client_addr.sin_port) << std::endl;

                addfd(epfd, clientfd, true);

                clients_list.push_back(clientfd);
                std::cout << "Add new client to epoll fd, the fd is : " << clientfd << std::endl;
                std::cout << "There are " << clients_list.size() << " clients are connected" << std::endl;

                // 服务端发送消息
                char message[MAX_MSG_SIZE] = {0};
                sprintf(message, "欢迎来到你大爷的服务器OvO玩的愉快哦\n");
                int ret = send(clientfd, message, strlen(message) + 1, 0);
                // std::cout << ret << std::endl;
                if (ret < 0)
                {
                    perror("send");
                    exit(-1);
                }
                else if (ret == 0)
                {
                    removefd(epfd, clientfd);
                    close(clientfd);
                    printf("发送消息失败\n");
                }
            }
            else
            {
                int ret = sendBroadcastmessage(sockfd);
                if (ret < 0)
                {
                    perror("send");
                    exit(-1);
                }
                else if (ret == 0)
                {
                    removefd(epfd, sockfd);
                    close(sockfd);
                    printf("发送消息失败\n");
                }
            }
        }
    }
    close(listenFd);
    close(epfd);

    return 0;
}