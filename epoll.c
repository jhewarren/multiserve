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

#define SERVERPORT "8000"
#define MAXEVENTS 1000


//gcc -Wall -fopenmp -ggdb -o epoll_server epoll.c lib/socketwrapper.c
/*
void close_fd (int signo, int fd_server)
{
        close(fd_server);
	exit (EXIT_SUCCESS);
}
*/
int main(int argc, char ** argv) {
    int sfd, s;
    int efd;
    struct epoll_event event;
    struct epoll_event *events;
//	struct sigaction act;
    epoll_data * data;
/*
	act.sa_handler = close_fd();
	act.sa_flags = 0;
	if ((sigemptyset (&act.sa_mask) == -1 || sigaction (SIGINT, &act, NULL) == -1))
	{
			perror ("Failed to set SIGINT handler");
			exit (EXIT_FAILURE);
	}
*/
    //make and bind the socket
    sfd = make_bound(SERVERPORT);
    if (sfd == -1) {
        return 2;
    }

    //start listening
    s = listen(sfd, SOMAXCONN);
    if (s == -1) {
        perror("listen");
        return 3;
    }

    //register the epoll structure
    efd = epoll_create1(0);
    if (efd == -1) {
        perror ("epoll_create1");
        return 4;
    }

    data = calloc(1, sizeof(epoll_data));
    epoll_data_init(data, sfd);
    event.data.ptr = data;
    event.events = EPOLLIN | EPOLLET | EPOLLEXCLUSIVE;
    s = epoll_ctl(efd, EPOLL_CTL_ADD, sfd, &event);
    if (s == -1) {
        perror("epoll_ctl");
        return 5;
    }

    // Buffer where events are returned (no more that 64 at the same time)
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
