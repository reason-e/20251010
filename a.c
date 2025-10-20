#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>

int Shutdown=1;
int client_array[1020];
int socket_init(void); // 返回socket
int return_response(int clientfd, const char *clientip);

//向新连接的客户端发送一条欢迎消息
int return_response(int clientfd, const char *clientip){
    char response[4096];
    bzero(response,sizeof(response));
    sprintf(response,"hi %s welcome test TCP server\n",clientip);
    send(clientfd,response,strlen(response),0);
    return 0;
}
int socket_init(void)
{ // 返回socket
    int sockfd;
    struct sockaddr_in sockaddr;
    bzero(&sockaddr, sizeof(sockaddr));
    sockaddr.sin_family = AF_INET;
    sockaddr.sin_port = htons(8080);
    sockaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("socket call failed");
        exit(0);
    }
    if ((bind(sockfd, (struct sockaddr *)&sockaddr, sizeof(sockaddr))) == -1)
    {
        perror("bind call failed");
        exit(0);
    }
    listen(sockfd, 128);
    return sockfd;
}

int main(void)
{
    int serverfd, maxfd, clientfd;
    int ready;
    struct sockaddr_in clientaddr;
    socklen_t addrlen;
    int recvlen;
    char recv_buf[1500];
    char client_ip[16];
    fd_set set, oset; // 传入传出分离
    bzero(recv_buf, sizeof(recv_buf));
    bzero(client_ip, sizeof(client_ip));
    serverfd = socket_init();
    FD_SET(serverfd, &set); // 设置监听
    for (int i = 0; i < 1020; i++)
        client_array[i] = -1;
    // maxfd
    maxfd = serverfd;
    while (Shutdown)
    {
        oset = set;
        int serverfd;
        if ((ready = select(maxfd + 1, &oset, NULL, NULL, NULL)) == -1)
        { // 阻塞监听事件
            perror("select call failed");
            exit(0);
        }
        while (ready)
        {
            // 辨别就绪
            if (FD_ISSET(serverfd, &oset))
            {
                addrlen = sizeof(clientaddr);
                if(clientfd = accept(serverfd, (struct sockaddr *)&clientaddr, &addrlen)==-1)
                {
                    perror("accept call failed");
                    exti(0);
                }
                inet_ntop(AF_INET,&clientaddr.sin_addr.s_addr,client_ip,16);
                printf("listen server socket success,call Accept,client ip %s,client port%d\n",client_ip,ntohs(clientaddr.sin_port));
                return_response(clientfd,client_ip);
                if (maxfd < clientfd)
                    maxfd = clientfd;
                for (int i = 0; i < 1020; i++)
                    if (client_array[i] == -1)
                    {
                        client_array[i] = clientfd;
                        break;
                    }
                FD_SET(clientfd,&set);

            }else{
   
            }
            --ready;
        }
    }
}
