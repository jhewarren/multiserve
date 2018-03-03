#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include "lib/socketwrapper.h"

#define SERVERPORT 8000
#define SERVER_IP "127.0.0.1"
#define LIMIT 10000

static char tBuf[BUFLEN];
// gcc -Wall -ggdb -o mec client.c

void *connection_handler(void *socket_desc)
{
    //Get the socket descriptor
    int *sock = (int*)socket_desc;
    int read_size,e;
    char *message = "Hi how are you!!!!";
    //int e;
    //Receive a message from client
    while(1)
    {
        if((e = SendMsg(*sock , message)) == -1)
			break;

		if((read_size = RecvMsg(*sock , message)) == -1)
			break;
		//Send the message back to client
		//clear the message buffer
		memset(message, 0, BUFLEN);
    }
    return 0;
}

int main(int argc, char ** argv){
    int port;
    char *host;

    switch(argc){
		case 1:
			host =	SERVER_IP;			// Host name
			port =	SERVERPORT;	// default tcp port number
		break;
		case 2:
			host =	argv[1];			// Host name
			port =	SERVERPORT;	// default tcp port number
		break;
		case 3:
			host =	argv[1];
			port =	atoi(argv[2]);		// User specified port
		break;
		default:
			fprintf(stderr, "Usage: %s host [port]\n", argv[0]);
			exit(1);
	}

    int socket_desc;
    struct sockaddr_in server;

    while((socket_desc = Socket(AF_INET , SOCK_STREAM , 0))!=-1)
    {
        ConfigClientSocket((struct sockaddr_in *)&server, host, port);

        Connect(socket_desc, (struct sockaddr_in)server);

        //Accept and incoming connection
        int *c = calloc(1,sizeof(struct sockaddr_in));
    	pthread_t thread_id;

        if( pthread_create( &thread_id , NULL ,  connection_handler , (void *)c)!=0)
        {
            perror("could not create thread");
            return 1;
        }
    }

    return 0;
}
