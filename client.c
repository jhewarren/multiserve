#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>

#define MAX_SIZE 4096
#define CLIENTS 5000
#define SERVER "127.0.0.1"
#define SPORT 8000

static char *server;
static int sport;
static int clients;

// gcc -Wall -ggdb -pthreads client.c -o mec

void *connection_handler(void *threadid){
    int threadnum = (int)threadid;
    int sock_desc,i=1;
    struct sockaddr_in serv_addr;
    char sbuff[MAX_SIZE],rbuff[MAX_SIZE];
    sprintf(sbuff,"%i> How's this for a buffer",threadnum);

    if((sock_desc = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        printf("Failed creating socket\n");

    bzero((char *) &serv_addr, sizeof (serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(server);
    serv_addr.sin_port = htons(sport);

    if (connect(sock_desc, (struct sockaddr *) &serv_addr, sizeof (serv_addr)) < 0) {
        printf("Failed to connect to server\n");
    }

    printf("Connected successfully client:%s initially\n", sbuff);
    while(1)
    {
        printf("Sending: %s%d", sbuff,(i++)%1000);
        send(sock_desc,sbuff,strlen(sbuff),0);

        if(recv(sock_desc,rbuff,MAX_SIZE,0)==0)
            printf("Error");
        else
//           fputs(rbuff,stdout);
            printf("\t\tRcvd: %lu\n",strlen(rbuff));

        bzero(rbuff,MAX_SIZE);
//        sleep(2);
    }
    close(sock_desc);
    return 0;
}

int main (int argc, char **argv){
    int i;
    pthread_t sniffer_thread;
    
    	switch(argc)	{
		case 1:
			server =	SERVER;     // Delfault Host  - loopback
			sport  =	SPORT;      // Default tcp port number
            clients =   CLIENTS;    // Default number of clients
		break;
		case 2:
			server =	argv[1];    // defined Host name
			sport  =	SPORT;      
            clients =   CLIENTS;
		break;
		case 3:
			server =	argv[1];
			sport  =	atoi(argv[2]);		// User specified port
            clients =   CLIENTS;
		break;
		case 4:
			server =	argv[1];
			sport  =	atoi(argv[2]);
            clients =   atoi(argv[3]);  // User specified number of clients
		break;
		default:
			fprintf(stderr, "Usage: %s [host port clients]\n", argv[0]);
			exit(1);
	}

    
    for (i=1; i<=clients; i++) {
        if( pthread_create( &sniffer_thread , NULL ,  connection_handler , (void*) i) < 0)
        {
            perror("could not create thread");
            return 1;
        }
    }
    pthread_exit(NULL);
    return 0;
}

