#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <omp.h>
#include <signal.h>
#include "lib/socketwrapper.h"

#define SERVERPORT "54321"
#define MAXEVENTS 1000


//gcc -Wall -fopenmp -ggdb -o epoll_server epoll.c lib/socketwrapper.c

int main(int argc, char ** argv) {
    int sfd, s;
    int efd;
    struct epoll_event event;
    struct epoll_event *events;
    epoll_data * data;

    //make and bind the socket
    if ((sfd = make_bound(SERVERPORT)) == -1) {
        return 2;
    }

    //start listening
    if ((s = listen(sfd, SOMAXCONN)) == -1) {
        perror("listen");
        return 3;
    }

    //register the epoll structure
    if ((efd = epoll_create1(0)) == -1) {
        perror ("epoll_create1");
        return 4;
    }

    data = calloc(1, sizeof(epoll_data));
    epoll_data_init(data, sfd);
    event.data.ptr = data;
    event.events = EPOLLIN | EPOLLET | EPOLLEXCLUSIVE;
    if ((s = epoll_ctl(efd, EPOLL_CTL_ADD, sfd, &event)) == -1) {
        perror("epoll_ctl");
        return 5;
    }

    events = calloc(MAXEVENTS, sizeof(event));

#pragma omp parallel
    while (1) {
        int n, i;

        n = epoll_wait(efd, events, MAXEVENTS, -1);
        for (i = 0; i < n; i++) {
            if ((events[i].events & EPOLLERR) || (events[i].events & EPOLLHUP)) {
                // A socket got closed
                epoll_data_close((epoll_data *)events[i].data.ptr);
                free(events[i].data.ptr);
                continue;
            } else if((events[i].events & EPOLLIN)) {
                if (sfd == ((epoll_data *)events[i].data.ptr)->fd) {
                    // We have a notification on the listening socket, which
                    // means one or more incoming connections.
                    while (1) {
                        struct sockaddr in_addr;
                        socklen_t in_len;
                        int infd;
                        char hbuf[NI_MAXHOST], sbuf[NI_MAXSERV];

                        in_len = sizeof in_addr;
                        infd = accept(sfd, &in_addr, &in_len);
                        if (infd == -1) {
                            if ((errno == EAGAIN) || (errno == EWOULDBLOCK)) {
                                break;
                            } else {
                                perror("accept");
                                break;
                            }
                        }

                        s = getnameinfo(&in_addr, in_len, hbuf, sizeof hbuf, sbuf, sizeof sbuf, NI_NUMERICHOST | NI_NUMERICSERV);
                        if (s == 0) {
                            printf("Accepted connection on descriptor %d "
                                    "(host=%s, port=%s)\n", infd, hbuf, sbuf);
                        }

                        // Make the server listening socket non-blocking
                        if (fcntl (infd, F_SETFL, O_NONBLOCK | fcntl (infd, F_GETFL, 0)) == -1)
                            perror("fcntl");

                        data = calloc(1, sizeof(epoll_data));
                        epoll_data_init(data, infd);

                        event.data.ptr = data;
                        event.events = EPOLLIN | EPOLLOUT | EPOLLET;
                        s = epoll_ctl(efd, EPOLL_CTL_ADD, infd, &event);
                        if (s == -1) {
                            perror("epoll_ctl");
                            abort();
                        }
                    }
                    continue;
                } else {
                    //regular incomming message echo it back
                    echo((epoll_data *)event.data.ptr);
                }
            } else {
                //EPOLLOUT
                echo_harder((epoll_data *)event.data.ptr);
            }
        }
    }
    free(events);
    close(sfd);
    return 0;
}