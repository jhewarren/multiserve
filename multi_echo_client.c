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
#include <strings.h>
#include <unistd.h>
#include <gmp.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/wait.h>
#include "lib/socketwrapper.h"

#define SERVER_TCP_PORT		7000		// Default port
#define PAYLOAD				800  		// Buffer length
#define SERVER_IP			127.0.0.1	// Default server IP

long delay (struct timeval t1, struct timeval t2);

int main (int argc, char **argv)
{
	int n, bytes_to_read;
	int sd, port, seq, payload, count = 0;
	struct hostent	*hp;
	struct sockaddr_in server;
	char  *host, *bp, rbuf[PAYLOAD], sbuf[PAYLOAD], **pptr;
	char str[16];
	struct time_val tstart, tend, tsend, treceive;
    double tlatest, tmin, tmax, tavg;

	switch(argc)
	{
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
			sequence =	atoi(argv[3]);	// User specified port
			payload = PAYLOAD;			// message payload in bytes
		break;
		case 5:
			host =	argv[1];
			port =	atoi(argv[2]);	// User specified port
			sequence =	atoi(argv[3]);	// User specified port
			payload =	atoi(argv[4]);	// User specified port
		break;
		default:
			fprintf(stderr, "Usage: %s host [port sequence payload ]\n", argv[0]);
			exit(1);
	}

	// Create the socket
	if ((sd = Socket(AF_INET, SOCK_STREAM, 0)) == -1)
		exit(1);

	ConfigServerSocket(&server,port);
	
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
	
	// get user's text
	strncopy(sbuf, '@', PAYLOAD);

	gettimeofday(&tstart,NULL);

	while (1){
        
        printf("Transmit:\n");
		// Transmit data through the socket
		gettimeofday(&tsend, NULL);

		send (sd, sbuf, PAYLOAD, 0);

		printf("Receive:\n");
		bp = rbuf;
		bytes_to_read = PAYLOAD;

		// client makes repeated calls to recv until no more data is expected to arrive.
		n = 0;
		while ((n = recv (sd, bp, bytes_to_read, 0)) < PAYLOAD)
		{
			bp += n;
			bytes_to_read -= n;
		}
		printf ("%s\n", rbuf);
		gettimeofday(&treceive, NULL);
		tlatest = delay(tsend, &treceive);
        if (count==0) 
            tmin=tmax=tavg=tlatest;
        else
        {
            tavg+=tlatest;
            if (tlatest<tmin)
                tmin=tlatest;
            if (tlatest>tmax)
                tmax=tlatest;
        }
        count ++;
		fflush(stdout);
	}
	gettimeofday(&tend,NULL);
    tavg/=count;
	close (sd);
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