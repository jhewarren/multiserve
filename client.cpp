#include <string.h>
#include <cstring>
#include <unistd.h>
#include <stdio.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <strings.h>
#include <stdlib.h>
#include <string>
#include <time.h>
#include <vector>
using namespace std;

int main (int argc, char* argv[])
{
	//TODO have increminting portNo 
	int listen_socket, portNo = 8001, socket_connection, echo_len;
	bool loop = false;
	struct sockaddr_in svrAdd;
	struct hostent *server;
	if(argc < 3)
	{
		cerr<<"Syntax : ./client <host name> <echo length>"<<endl;
		return 0;
	}

	//create string
	echo_len = atoi(argv[2]);
	string echo = string(echo_len,'a');

	//create client skt
	if((listen_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
	{
		cerr << "Cannot open socket" << endl;
		return 0;
	}
	
	if((server = gethostbyname(argv[1])) == NULL)
	{
		cerr << "Host does not exist" << endl;
		return 0;
	}
	
	bzero((char *) &svrAdd, sizeof(svrAdd));
	svrAdd.sin_family = AF_INET;
	
	bcopy((char *) server -> h_addr, (char *) &svrAdd.sin_addr.s_addr, server -> h_length);
	
	svrAdd.sin_port = htons(portNo);
	
	if ((socket_connection = connect(listen_socket,(struct sockaddr *) &svrAdd, sizeof(svrAdd))) < 0)
	{
		cerr << "Cannot connect!" << endl;
		return 0;
	}
	
	for(;;)
	{
		write(listen_socket, &echo, echo_len);
		bzero(&echo, 300);
		read(listen_socket, &echo, echo_len);
	}
}