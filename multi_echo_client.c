/*---------------------------------------------------------------------------------------
--	SOURCE FILE:		multi_echo_client.c - A simple TCP client program.
--
--	PROGRAM:		multi_echo_client
--				gcc -Wall -ggdb -o mec multi_echo_client.c
--
--	FUNCTIONS:		Berkeley Socket API
--
--	DATE:			February 22, 2018
--
--	REVISIONS:		(Date and Description)
--				January 2005
--				Modified the read loop to use fgets.
--				While loop is based on the buffer length
--
--
--	DESIGNERS:		Aman Abdulla, John Warren
--
--	PROGRAMMERS:		Aman Abdulla, John Warren
--
--	NOTES:
--	The program will establish a TCP connection to a user specifed server.
-- 	The server can be specified using a fully qualified domain name or and
--	IP address.
--  After the connection has been established a defined buffer is sent to
--  the server and the response (echo) back from the server is displayed.
--	This client application can be used to test the accompanying epoll
--	server: epoll_svr.c
---------------------------------------------------------------------------------------*/
#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <gmp.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/wait.h>
#include "lib/socketwrapper.h"

#define SERVER_TCP_PORT		7000		// Default port
#define PAYLOAD				80  		// Buffer length
#define SERVER_IP			"127.0.0.1"	// Default server IP

// long delay (struct timeval t1, struct timeval t2);

int main (int argc, char **argv)
{
	int n, bytes_to_read;
	int sd, port, seq, payload, count = 0;
	struct hostent	*hp;
	struct sockaddr_in server;
	char  *host, *bp, rbuf[PAYLOAD], sbuf[PAYLOAD], myBuf[]="This is a test of the emergency broadcast system.", **pptr;
	char str[16];
//	struct time_val tstart, tend, tsend, treceive;
//    double tlatest, tmin, tmax, tavg, tnet;
//    sbuf;

	switch(argc)
	{
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
			port =	atoi(argv[2]);		// User specified port
			seq = 1;					// client ID
			payload = PAYLOAD;			// message payload in bytes
		break;
		case 4:
			host =	argv[1];
			port =	atoi(argv[2]);		// User specified port
			seq =	atoi(argv[3]);	// User specified port
			payload = PAYLOAD;			// message payload in bytes
		break;
		case 5:
			host =	argv[1];
			port =	atoi(argv[2]);	// User specified port
			seq =	atoi(argv[3]);	// User specified port
			payload =	atoi(argv[4]);	// User specified port
		break;
		default:
			fprintf(stderr, "Usage: %s host [port seq payload ]\n", argv[0]);
			exit(1);
	}

	while (1){
	// Create the socket
	if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
		exit(1);

	bzero((char *)&server, sizeof(struct sockaddr_in));
	server.sin_family = AF_INET;
	server.sin_port = htons(port);
//	if ((hp = gethostbyname(host)) == NULL)
//    	ConfigServerSocket(&server,port);

	if ((hp = gethostbyname(host)) == NULL)
	{
		fprintf(stderr, "Unknown server address\n");
		exit(1);
	}
	bcopy(hp->h_addr, (char *)&server.sin_addr, hp->h_length);

	// Connecting to the server
	if (connect(sd, (struct sockaddr *)&server, sizeof(server)) == -1)
	{
		fprintf(stderr, "Can't connect to server\n");
		perror("connect");
		exit(1);
	}
	printf("Connected:    Server Name: %s\n", hp->h_name);
	pptr = hp->h_addr_list;
	printf("\t\tIP Address: %s\n", inet_ntop(hp->h_addrtype, *pptr, str, sizeof(str)));


//	clock_gettime(&tstart,NULL);

        bcopy(myBuf, sbuf, PAYLOAD);
        sbuf[count]='0';

		printf ("%s\n", sbuf);
        printf("Transmit:\n");
		// Transmit data through the socket
//		clock_gettime(&tsend, NULL);

		send (sd, sbuf, sizeof(sbuf), 0);
//		SendMsg (sd, sbuf);

		printf("Receive:\n");
		bp = rbuf;
		bytes_to_read = PAYLOAD;

		// client makes repeated calls to recv until no more data is expected to arrive.
		n = 0;
//		while ((n = recv (sd, bp, bytes_to_read, 0)) < PAYLOAD)
		while ((n = recv (sd, bp, bytes_to_read, 0)) >0)
//		RecvMsg (sd, bp);
//		{
//			bp += n;
//			bytes_to_read -= n;
//		}
		printf ("%s\n", rbuf);
//		clock_gettime(&treceive, NULL);
//		tlatest = delay(&tsend, &treceive);
 //       if (count==0)
 //           tmin=tmax=tavg=tlatest;
  //      else
//        {
//            tavg+=tlatest;
//            if (tlatest<tmin)
//                tmin=tlatest;
//            if (tlatest>tmax)
//                tmax=tlatest;
//        }
        count ++;
	}
	fflush(stdout);
//	clock_gettime(&tend,NULL);
//	tnet = delay(&tstart, &tend);
//    tavg/=count;
	close (sd);
    printf("%d %d %d", payload, seq, count);
//    printf("%d: %d usec total - %d messages %d total bytes - %d|%d|%d (min|max|avg)",seq, tnet, count, PAYLOAD*count,tmin, tmax, tavg);
	return (0);
}

// Compute the delay between tl and t2 in milliseconds
long delay (struct timeval t1, struct timeval t2)
{
	long d;

	d = (t2.tv_sec - t1.tv_sec) * 1000;
	d += ((t2.tv_usec - t1.tv_usec + 500) / 1000);
	return(d);
}
