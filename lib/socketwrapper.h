#ifndef SOCKETWRAPPERS_H
#define SOCKETWRAPPERS_H

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <limits.h>
#define BUFLEN 4096 //should be the same on client size

typedef struct {
    int fd;
    int pipefd[2];
} epoll_data;

int Socket(int family, int type, int protocol);
void SetReuse (int socket);
int Bind(int socket, struct sockaddr_in *addr);
int Listen(int socket, int size);
int Accept(int socket, struct sockaddr *addr, socklen_t *addrlen);
void ConfigServerSocket(struct sockaddr_in *servaddr, int port);
void ConfigClientSocket(struct sockaddr_in *servaddr, const char* ip, int port);
void Connect(int sockfd, struct sockaddr_in sockaddr);
void SendMsg(int sockfd, char* buffer);
int RecvMsg(int sockfd, char* buffer);
int make_bound(const char *port);
int make_non_blocking(int sfd);
//echo
int echo(epoll_data * epd);
int echo_harder(epoll_data * epd);
//close all the members
void epoll_data_close(epoll_data * epd);
//initialize the pip and set the fd
int epoll_data_init(epoll_data * epd, int fd);
#endif //SOCKETWRAPPERS_H

#ifndef SPLICE_F_MOVE
#define SPLICE_F_MOVE           0x01
#endif
#ifndef SPLICE_F_NONBLOCK
#define SPLICE_F_NONBLOCK       0x02
#endif
#ifndef SPLICE_F_MORE
#define SPLICE_F_MORE           0x04
#endif
#ifndef SPLICE_F_GIFT
#define SPLICE_F_GIFT           0x08
#endif