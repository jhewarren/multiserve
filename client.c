#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>

#define MAX_SIZE 4096
#define CLIENTS 5
#define SERVER "127.0.0.1"
#define SPORT 8000
#define MESSAGES 1000

static char *server;
static int sport;
static int clients;
static int messages;

// gcc -Wall -ggdb -pthreads client.c -o mec
// tcpdump -i eth0 host 192.168.1.3 and port 5060 -n -s 0 -vvv -w /usr/src/dump
// - nload is the full card speeds
// - iftop is per destination
// - nethogs is per program

void *connection_handler(void *threadid){
    int threadnum = (int)threadid;
    int sock_desc, j;
    struct sockaddr_in serv_addr;
    char sbuff[MAX_SIZE],rbuff[MAX_SIZE];
    struct timespec tstart;
    struct timespec tend;
    float dconn, decho, tmax=0.0, tmin=999.9, tavg=0.0;

    sprintf(sbuff,"COMP 8006 Computer Systems Technology March 2018 - Network Administration and Security Level 2 - Assignment #3 - Due: March 8, 2018 – 1300 hrs. You may work in groups of two. Objective: To design, implement and test a simple monitor application that will detect password guessing attempts against a service and block that IP using Netfilter. Assignment: Design, implement and test an application that will monitor the /var/log/secure file and detect password guessing attempts and then use iptables to block that IP.  Your application will get user specified parameters (see constraints) and then continuously monitor the log file specified.  As soon as the monitor detects that the number of attempts from a particular IP has gone over a user-specified threshold, it will generate a rule to block that IP.  If the user has specified a time limit for a block, your application will flush the rule from Firewall rule set upon expiration of the block time limit. Design a test procedure that will test your application under a variety of conditions. For example, how will you handle a situation when an attacker sends a slow scan of your system, meaning several password guessing attempts, but spaced far enough apart in time so that your application will miss the attack.Constraints: The application will be implemented using any scripting or programming language of your choice. The Firewall rules will be implemented using Netfilter. Your application will obtain user input for the following parameters: The number of attempts before blocking the IP The time limit for blocking the IP. The default setting will be block indefinitely. Monitor a log file of user’s choice (Optional - bonus). Keep in mind that different log files have different formats.  Your application will be activated through the crontab. To Be Submitted:  Hand in complete and well-documented design work and the firewall script. You are also required to demonstrate your working programs during the lab the day the assignment is due. A formal and detailed test plan as well as the test results for each test case. Provide your test data and code and all supporting documentation. Include a set of instructions on how to use your application. Essentially a small HOW-TO Submit a zip file containing all the code and documents as described below in the sharein folder for this course under Assignment #3. Evaluation:(1). Design/Documentation: / 5 (2). Functionality: / 30 (3). Testing: / 15 Total: / 50");

    if((sock_desc = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        printf("Failed creating socket\n");

    bzero((char *) &serv_addr, sizeof (serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(server);
    serv_addr.sin_port = htons(sport);

    clock_gettime(CLOCK_MONOTONIC, &tstart);
    if (connect(sock_desc, (struct sockaddr *) &serv_addr, sizeof (serv_addr)) < 0) {
        printf("Failed to connect to server\n");
    }
    clock_gettime(CLOCK_MONOTONIC, &tend);
    dconn =     ((double)tend.tv_sec + 1.0e-9*tend.tv_nsec) -
    ((double)tstart.tv_sec + 1.0e-9*tstart.tv_nsec);

    printf("Connected #%d in %fs\n", threadnum, dconn);

    for (j=1;j<=messages;j++)
    {
//        printf("Sending: %s%d", sbuff,(i++)%1000);
        send(sock_desc,sbuff,strlen(sbuff),0);
        clock_gettime(CLOCK_MONOTONIC, &tstart);

        if(recv(sock_desc,rbuff,MAX_SIZE,0)==0)
            printf("Error");

        clock_gettime(CLOCK_MONOTONIC, &tend);
        decho = ((double)tend.tv_sec + 1.0e-9*tend.tv_nsec) -
        ((double)tstart.tv_sec + 1.0e-9*tstart.tv_nsec);

        if(decho<tmin)
            tmin=decho;
        if(decho>tmax)
            tmax=decho;
        tavg=(tavg*(j-1)/j)+(decho/j);
//        printf("%lu ",strlen(rbuff)/strlen(sbuff));

//        printf("Client %d #%d Tx/Rx: %lu/%lu in %fs\n",threadnum, j, strlen(sbuff),strlen(rbuff),decho );
        bzero(rbuff,MAX_SIZE);
    }
    // client N; x messages sent/received in min/max/avg
    printf("\n\nClient %d connected in %f; %d messages in %f/%f/%fs\n\n",threadnum, dconn, j-1,tmin,tmax,tavg);
    close(sock_desc);
    return 0;
}

int main (int argc, char **argv){
    int i;
    pthread_t sniffer_thread;

    	switch(argc)	{
/*		case 1:
			server =	SERVER;     // Delfault Host  - loopback
			sport  =	SPORT;      // Default tcp port number
            clients =   CLIENTS;    // Default number of clients
            messages =  MESSAGES;   // Default number of messages
		break;*/
		case 2:
			server =	argv[1];    // defined Host name
			sport  =	SPORT;
            clients =   CLIENTS;
            messages =  MESSAGES;   // Default number of messages
		break;
		case 3:
			server =	argv[1];
			sport  =	atoi(argv[2]);		// User specified port
            clients =   CLIENTS;
            messages =  MESSAGES;   // Default number of messages
		break;
		case 4:
			server =	argv[1];
			sport  =	atoi(argv[2]);
            clients =   atoi(argv[3]);  // User specified number of clients
            messages =  MESSAGES;   // Default number of messages
		break;
        case 5:
			server =	argv[1];
			sport  =	atoi(argv[2]);
            clients =   atoi(argv[3]);  // User specified number of clients
            messages =  atoi(argv[4]);   // Default number of messages
		break;
		default:
			fprintf(stderr, "Usage: %s [host port clients messages]\n", argv[0]);
			exit(1);
	}
//    printf("\n\tCurrent Config: %s:%d x %d\n\n",server, sport, clients);


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
