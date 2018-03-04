#include "lib/socketwrapper.h"
#include <poll.h>

#define SERV_PORT 8000
#define OPEN_MAX 5000
#define LISTENQ 5
#define TIMEOUT -1
//#define NOTDEF 1 //debug
//gcc -Wall -fopenmp -ggdb -o poll_server poll_server.c lib/socketwrapper.c
int main(int argc, char **argv)
{
	int i, maxi, listenfd, connfd, sockfd;
	int nready;
	ssize_t n;
	char buf[BUFLEN];
	socklen_t clilen;
	struct pollfd client[OPEN_MAX];
	struct sockaddr_in cliaddr, servaddr;

	//struct rlimit lim;
    //lim.rlim_cur = (1UL << 20);
    //lim.rlim_max = (1UL << 20);
    //setrlimit(RLIMIT_NOFILE, &lim);

	listenfd = Socket(AF_INET, SOCK_STREAM, 0);
	ConfigServerSocket(&servaddr,SERV_PORT);
	Bind(listenfd, &servaddr);
	Listen(listenfd, LISTENQ);

	client[0].fd = listenfd;
	client[0].events = POLLRDNORM;
	
	for(i = 1; i < OPEN_MAX; i++)
	{
		// -1 indicates available entry
		client[i].fd = -1;
	}

	maxi = 0; // max index into client[] array 
	while(1)
	{
		nready = poll(client, maxi + 1, TIMEOUT);

		if(client[0].revents & POLLRDNORM)
		{
			// new client connection 
			clilen = sizeof(cliaddr);
			connfd = Accept(listenfd, (struct sockaddr *)&cliaddr, &clilen);
			#ifdef NOTDEF
			printf("new client: %s\n", inet_ntoa(cliaddr.sin_addr));
			#endif

			for(i = 1; i < OPEN_MAX; i++){
				if(client[i].fd < 0){
					client[i].fd = connfd; // save descriptor 
					break;
				}
			}

			if(i == OPEN_MAX)
				perror("too many clients");

			client[i].events = POLLRDNORM;
			if(i > maxi)
				maxi = i; // max index in client[] array 

			if(--nready <= 0)
				continue; // no more readable descriptors 
		}

		// check all clients for data 
		for(i = 1; i <= maxi; i++){ 
			if((sockfd = client[i].fd) < 0)
				continue;
			
			if(client[i].revents & (POLLRDNORM | POLLERR)){
				if((n = read(sockfd, buf, BUFLEN)) < 0){
					if(errno == ECONNRESET){
						//connection reset by client 
						#ifdef NOTDEF
						printf("client[%d] aborted connection\n", i);
						#endif
						close(sockfd);
						client[i].fd = -1;
					}else{
						perror("read error");
					}
				}else if(n == 0){
					//connection closed by client 
					#ifdef NOTDEF
					printf("client[%d] closed connection\n", i);
					#endif
					close(sockfd);
					client[i].fd = -1;
				}else{
					write(sockfd, buf, n);
				}

				if(--nready <= 0){
					break; // no more readable descriptors 
				}
			}
		}
	}
}