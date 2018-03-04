/*------------------------------------------------------------------------------------------------
-- SOURCE FILE: socketwrappers.cpp
-- PROGRAM:     COMP7005 - Assignment 1 FTP
-- FUNCTIONS:   int Socket(int family, int type, int protocol);
--				void SetReuse (int socket);
--				int Bind(int socket, struct sockaddr_in *addr);
--				int Listen(int socket, int size);
--				int Accept(int socket, struct sockaddr *addr, socklen_t *addrlen);
--				void ConfigServerSocket(struct sockaddr_in *servaddr, int port);
--				void ConfigClientSocket(struct sockaddr_in *servaddr, const char* ip, int port);
--				bool Connect(int sockfd, struct sockaddr_in sockaddr);
--				bool SendMsg(int sockfd, char* buffer);
--				int RecvMsg(int sockfd, char* buffer);
--				Cmd RecvCmd(int sockfd);
--				Cmd CreateCmd(int type, char *filename);
--				void SendFile(int socket, char *filename);
-- DATE:        Oct 2, 2017
-- DESIGNER:    Aing Ragunathan
-- PROGRAMMER:  Aing Ragunathan
-- EDDITED:		Peyman Tehrani Parsa
-- NOTES:       Socket wrappers used for client and server communication in assignment 1
--------------------------------------------------------------------------------------------------*/
#include "socketwrapper.h"
/*-----------------------------------------------------------------------------------------------
-- FUNCTION:   Socket
-- DATE:       Oct 2, 2017
-- PROGRAMMER: Aing Ragunathan
-- INTERFACE:  int Socket(int family, int type, int protocol)
-- PARAMETER:  	int family 		- communication domain
--				int type		- communication semantics
--				int protocol	- type of protocol to be used with the socket
-- RETURNS:    a integer referring to the socket identifer
-- NOTES:      Socket wrapper for creating and validating the creation of a socket
----------------------------------------------------------------------------------------------- */
int Socket(int family, int type, int protocol) {
	int n;
	if((n = socket(family, type, protocol)) < 0) {
		perror("socket error\n");
		return -1;
	}
	return n;
}

/*-----------------------------------------------------------------------------------------------
-- FUNCTION:   SetReuse
-- DATE:       Oct 2, 2017
-- PROGRAMMER: Aing Ragunathan
-- INTERFACE:  void SetReuse(int socket)
-- PARAMETER:  	int socket 		- socket descriptor
-- RETURNS:    void
-- NOTES:      Sets a socket's option so it can be reused. Otherwise a wait may be forced.
----------------------------------------------------------------------------------------------- */
void SetReuse(int socket) {
	int yes;
	if(setsockopt(socket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
		perror("setsockopt");
	}
}

/*-----------------------------------------------------------------------------------------------
-- FUNCTION:   Bind
--
-- DATE:       Oct 2, 2017
-- PROGRAMMER: Aing Ragunathan
--
-- PARAMETER:  	int socket 					- socket descriptor
--				struct sockaddr_in *addr 	- address structure to bind the socket to
-- RETURNS:    returns 0 on success and -1 on failure
-- NOTES:      wrapper function for binding a socket to an address and validating the results
----------------------------------------------------------------------------------------------- */
int Bind(int socket, struct sockaddr_in *addr) {
	int n;
	if((n = bind(socket, (struct sockaddr*)addr, sizeof(struct sockaddr_in))) < 0) {
		perror("binding error");
		close(socket);
		return -1;
	}
	return n;
}

/*-----------------------------------------------------------------------------------------------
-- FUNCTION:   Listen
-- DATE:       Oct 2, 2017
-- PROGRAMMER: Aing Ragunathan
--
-- INTERFACE:  int Listen(int socket, int size)
-- PARAMETER:  	int socket 	- socket descriptor
--				int size	- max number the queue of pending connections can grow to
-- RETURNS:    returns 0 on success and -1 on failure
-- NOTES:      wrapper function for listening to a socket for a client's initiation of a handshake
----------------------------------------------------------------------------------------------- */
int Listen(int socket, int size) {
	int n;
	if((n = listen(socket, size)) < 0){
		perror("listening error\n");
		close(socket);
		return -1;
	}
	return 0;
}

/*-----------------------------------------------------------------------------------------------
-- FUNCTION:   Accept
-- DATE:       Oct 2, 2017
-- PROGRAMMER: Aing Ragunathan
--
-- INTERFACE:  int Accept(int socket, struct sockaddr *addr, socklen_t *addrlen)
-- PARAMETER:  	int socket 				- socket descriptor
--				struct sockaddr *addr	- address structure to accept a connection from
--				socklen_t *addrlen		- the length of the address structure
-- RETURNS:    returns the file descriptor of the connected client on success and -1 on failure
-- NOTES:      wrapper function for accepting to a connection from a client, with validation
----------------------------------------------------------------------------------------------- */
int Accept(int socket, struct sockaddr *addr, socklen_t *addrlen) {
	int n;
	printf("waiting for connection\n");
	if((n = accept(socket, addr, addrlen)) < 0) {
		perror("accept error\n");
		close(socket);
		return -1;
	}
	printf("Accepted connection\n");
	return n;
}

/*-----------------------------------------------------------------------------------------------
-- FUNCTION:   ConfigServerSocket
-- DATE:       Oct 2, 2017
-- PROGRAMMER: Aing Ragunathan
--
-- INTERFACE:  void ConfigServerSocket(struct sockaddr_in *servaddr, int port)
-- PARAMETER:  	struct sockaddr_in *servaddr 	- address structure to configure
--				int port						- port to configure the address structure to
-- RETURNS:    void
-- NOTES:      wrapper function for configuring a address structure for server use with validation
----------------------------------------------------------------------------------------------- */
void ConfigServerSocket(struct sockaddr_in *servaddr, int port) {
	bzero((char*)servaddr, sizeof(struct sockaddr_in));
	servaddr->sin_family = AF_INET;
	servaddr->sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr->sin_port = htons (port);
}

/*-----------------------------------------------------------------------------------------------
-- FUNCTION:   ConfigClientSocket
-- DATE:       Oct 2, 2017
-- PROGRAMMER: Aing Ragunathan
--
-- INTERFACE:  void ConfigClientSocket(struct sockaddr_in *servaddr, const char* ip, int port)
-- PARAMETER:  	struct sockaddr_in *servaddr 	- address structure to configure
--				const char* ip					- ip address to configure the address structure to
--				int port						- port to configure the address structure to
-- RETURNS:    void
-- NOTES:      wrapper function for configuring a address structure for client use with validation
----------------------------------------------------------------------------------------------- */
void ConfigClientSocket(struct sockaddr_in *servaddr, const char* ip, int port) {
	struct hostent* hp;
	if((hp = gethostbyname(ip)) == NULL) {
		perror("Unknown server address\n");
	}
	bzero((char*)servaddr, sizeof(struct sockaddr_in));
	servaddr->sin_family = AF_INET;
	servaddr->sin_port = htons (port);
	bcopy(hp->h_addr, (char*) &servaddr->sin_addr, hp->h_length);
}

/*-----------------------------------------------------------------------------------------------
-- FUNCTION:   Connect
-- DATE:       Oct 2, 2017
-- PROGRAMMER: Aing Ragunathan
-- INTERFACE:  bool Connect(int sockfd, struct sockaddr_in sockaddr)
-- PARAMETER:  	int sockfd 					- file descriptor to connect with
--				struct sockaddr_in sockaddr - address structure to connecto to
-- RETURNS:    return true if successful
-- NOTES:      wrapper function for connecting to a server with validation, for client user
----------------------------------------------------------------------------------------------- */
void Connect(int sockfd, struct sockaddr_in sockaddr) {
	if(connect(sockfd, (struct sockaddr *)&sockaddr, sizeof(sockaddr)) == -1)
		perror("Client: Can't connect to server\n");
	printf("Client: Connected!\n");
}

/*-----------------------------------------------------------------------------------------------
-- FUNCTION:   SendMsg
-- DATE:       Oct 2, 2017
-- PROGRAMMER: Aing Ragunathan
-- INTERFACE:  bool SendMsg(int sockfd, char* buffer)
-- PARAMETER:  	int sockfd 		- file descriptor to connect with
--				char* buffer 	- char array to send through the socket
-- RETURNS:    return true if successful
--
-- NOTES:      wrapper function for sending a char array with validation
----------------------------------------------------------------------------------------------- */
int SendMsg(int sockfd, char* buffer) {
	if(send(sockfd, buffer, BUFLEN, 0) == -1) {
		perror("SendMsg Failed\n");
	}
    return 1;
}

/*-----------------------------------------------------------------------------------------------
-- FUNCTION:   RecvMsg
-- DATE:       Oct 2, 2017
-- PROGRAMMER: Aing Ragunathan
-- INTERFACE:  int RecvMsg(int sockfd, char* buffer)
-- PARAMETER:  	int sockfd 		- file descriptor to connect with
--				char* buffer 	- char array to send through the socket
-- RETURNS:    return true if successful
-- NOTES:      wrapper function for receiving a char array with validation
----------------------------------------------------------------------------------------------- */
int RecvMsg(int sockfd, char* buffer) {
	int bytesRecv;

	if((bytesRecv = recv(sockfd, buffer, BUFLEN, 0)) == -1) {
		perror("RecvMsg Failed");
		return -1;
	}
	return bytesRecv;
}

int make_bound(const char * port) {
    struct addrinfo hints;
    struct addrinfo *result, *rp;
    int s, sfd;

    memset(&hints, 0, sizeof (struct addrinfo));
    hints.ai_family = AF_UNSPEC;     // Return IPv4 and IPv6 choices
    hints.ai_socktype = SOCK_STREAM; // We want a TCP socket
    hints.ai_flags = AI_PASSIVE;     // All interfaces

    s = getaddrinfo(0, port, &hints, &result);
    if (s != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror (s));
        return -1;
    }

    for (rp = result; rp != 0; rp = rp->ai_next) {
        sfd = socket(rp->ai_family, rp->ai_socktype | SOCK_NONBLOCK, rp->ai_protocol);
        if (sfd == -1)
            continue;

        int enable = 1;
        if (setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) == -1) {
            perror("Socket options failed");
            exit(EXIT_FAILURE);
        }

        s = bind(sfd, rp->ai_addr, rp->ai_addrlen);
        if (s == 0) {
            // We managed to bind successfully!
            break;
        }

        close(sfd);
    }
    if (!rp) {
        fprintf(stderr, "Unable to bind\n");
        return -1;
    }
    freeaddrinfo(result);
    return sfd;
}

int echo(epoll_data * epd) {
    while (1) {
        //read max standard pipe allocation size
        int nr = splice(epd->fd, 0, epd->pipefd[1], 0, USHRT_MAX, SPLICE_F_MOVE | SPLICE_F_MORE | SPLICE_F_NONBLOCK);
        if (nr == -1 && errno != EAGAIN) {
            perror("splice");
        }
        if (nr <= 0) {
            break;
        }
        printf("read: %d\n", nr);
        do {
            int ret = splice(epd->pipefd[0], 0, epd->fd, 0, nr, SPLICE_F_MOVE | SPLICE_F_MORE | SPLICE_F_NONBLOCK);
            if (ret <= 0) {
                if (ret == -1 && errno != EAGAIN) {
                    perror("splice2");
                }
                break;
            }
            printf("wrote: %d\n", ret);
            nr -= ret;
        } while (nr);
    }
    return 0;
}

int echo_harder(epoll_data * epd) {
    while (1) {
        int ret = splice(epd->pipefd[0], 0, epd->fd, 0, USHRT_MAX, SPLICE_F_MOVE | SPLICE_F_MORE | SPLICE_F_NONBLOCK);
        if (ret <= 0) {
            if (ret == -1 && errno != EAGAIN) {
                perror("splice");
            }
            break;
        }
        printf("wrote: %d\n", ret);
    }
    return 1;
}

int epoll_data_init(epoll_data * epd, int fd) {
    epd->fd = fd;
    if (pipe(epd->pipefd)) {
        perror("pipe");
        return 1;
    }
    return 0;
}

void epoll_data_close(epoll_data * epd) {
    close(epd->pipefd[0]);
    close(epd->pipefd[1]);
    close(epd->fd);
}