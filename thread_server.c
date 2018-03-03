#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<pthread.h>
#include <signal.h>
#include "lib/socketwrapper.h"

#define SERVERPORT 8000
#define LIMIT 10000
//gcc thread_server.c lib/socketwrapper.c -lpthread -o thread_server
void *connection_handler(void *socket_desc)
{

    //Get the socket descriptor
    int sock = *(int*)socket_desc;
    int read_size,e;
    char *message;
    //Receive a message from client
    while(1)
    {
		if((read_size = RecvMsg(sock , message)) == -1)
			break;
		//Send the message back to client
        if((e = SendMsg(sock , message)) == -1)
			break;

		//clear the message buffer
		memset(message, 0, BUFLEN);
    }
    return 0;
}

int main(int argc , char *argv[])
{
    int socket_desc , *client_sock;
    struct sockaddr_in server , client;

    socket_desc = Socket(AF_INET , SOCK_STREAM , 0);

	ConfigServerSocket((struct sockaddr_in *)&server,SERVERPORT);

    Bind(socket_desc, (struct sockaddr_in *)&server);

    Listen(socket_desc , LIMIT);

    //Accept and incoming connection
    client_sock = calloc(1,sizeof(struct sockaddr_in));
	pthread_t thread_id;

    while( (client_sock = Accept(socket_desc, (struct sockaddr *)client, (socklen_t*)client_sock)) )
    {
        if( pthread_create( &thread_id , NULL ,  connection_handler , (void*) client_sock) != 0)
        {
            perror("could not create thread");
            return 1;
        }
    }

    if (client_sock < 0)
    {
        perror("accept failed");
        return 2;
    }

    return 0;
}
