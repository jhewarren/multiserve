#define _GNU_SOURCE
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <sys/sysinfo.h>
#include <sys/wait.h>
#include <netdb.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <signal.h>
#include <unistd.h>
#include "lib/socketwrapper.h"

// gcc -Wall -ggdb -o mec client.c

int main(int argc, char ** argv){
    int port, seq, payload, rate=500;
    char *host, *port;
    struct addrinfo hints;
    struct addrinfo *result, *rp;
    int s, sfd;
    int total_threads = get_nprocs();
    epollfds = calloc(total_threads, sizeof(int));
    thread_cvs = calloc(total_threads, sizeof(pthread_cond_t));
    thread_mts = calloc(total_threads, sizeof(pthread_mutex_t));
    pthread_t * thread_fds = calloc(total_threads, sizeof(pthread_t));
    connection * con;
    struct epoll_event event;
    int epoll_pos = 0;

    switch(argc){
		case 1:
			host =	SERVER_IP;			// Host name
			port =	SERVER_TCP_PORT;	// default tcp port number
			seq = 1;					// client ID
			payload = PAYLOAD;			// message payload in bytes
		break;
		case 2:
			host =	argv[1];			// Host name
			port =	SERVER_TCP_PORT;	// default tcp port number
			seq = 1;					// client ID
			payload = PAYLOAD;			// message payload in bytes
		break;
		case 3:
			host =	argv[1];
			port =	argv[2];		// User specified port
			seq = 1;					// client ID
			payload = PAYLOAD;			// message payload in bytes
		break;
		case 4:
			host =	argv[1];
			port =	argv[2];		// User specified port
			seq =	atoi(argv[3]);	// User specified port
			payload = PAYLOAD;			// message payload in bytes
		break;
		case 5:
			host =	argv[1];
			port =	argv[2];	// User specified port
			seq =	atoi(argv[3]);	// User specified port
			payload =	atoi(argv[4]);	// User specified port
		break;
		default:
			fprintf(stderr, "Usage: %s host [port seq payload ]\n", argv[0]);
			exit(1);
	}

    //make the epolls for the threads
    //then pass them to each of the threads
    for (int i = 0; i < total_threads; ++i) {
        epollfds[i] = epoll_create1(0);
        pthread_cond_init(thread_cvs + i, 0);
        pthread_mutex_init(thread_mts + i, 0);
        pthread_attr_t attr;
        pthread_attr_init(&attr);
        int * thread_num = malloc(sizeof(int));
        *thread_num = i;
        pthread_create(thread_fds+i, &attr, &client_handler, (void*)thread_num);
        pthread_attr_destroy(&attr);
        printf("thread %d on epoll fd %d\n", i, epollfds[i]);
    }
    
    struct timespec ts;
    ts.tv_nsec = rate * 1000000;
    ts.tv_sec = 0;
    while (1) {
        nanosleep(&ts, 0);
        con = (connection *)malloc(sizeof(connection));
        init_connection(con, make_connected(address, port));

        set_non_blocking(con->sockfd);
        //enable_keepalive(con->sockfd);
        set_recv_window(con->sockfd);

        //cant add EPOLLRDHUP as EPOLLEXCLUSIVE would then fail
        //instead check for a read of 0
        event.data.ptr = con;

        //round robin client addition
        event.events = EPOLLET | EPOLLEXCLUSIVE | (!m||epoll_pos<1024?EPOLLIN | EPOLLOUT:0);
        epoll_ctl(epollfds[epoll_pos % total_threads], EPOLL_CTL_ADD, con->sockfd, &event);
        if (epoll_pos < total_threads) {
            pthread_cond_signal(&thread_cvs[epoll_pos]);
        }
        ++epoll_pos;
    }

    for(int i = 0; i < total_threads; ++i) {
        pthread_join(thread_fds[i],0);
    }
}