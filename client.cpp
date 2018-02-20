#include "client.h"
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
	
	ConfigClientSocket(&svrAdd,argv[1],portNo);
	
	Connect(listen_socket, svrAdd);
	
	for(;;)
	{
		//
		write(listen_socket, &echo, echo_len);
		bzero(&echo, 300);
		read(listen_socket, &echo, echo_len);
	}
}