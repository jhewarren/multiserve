#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/resource.h>

//the thread function
void *connection_handler(void *);

int main(int argc, char *argv[])
{
    int socket_desc, client_sock, c, *new_sock, clients = 0;
    struct sockaddr_in server, client;
    pthread_t sniffer_thread[65535];

    struct rlimit lim;
    lim.rlim_cur = (1UL << 20);
    lim.rlim_max = (1UL << 20);
    setrlimit(RLIMIT_NOFILE, &lim);

    if ((socket_desc = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        printf("Could not create socket");
    }

    //Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(8000);

    if (bind(socket_desc, (struct sockaddr *)&server, sizeof(server)) < 0)
    {
        perror("bind failed. Error");
        return 1;
    }

    listen(socket_desc, SOMAXCONN);

    printf("Waiting connections");
    c = sizeof(struct sockaddr_in);

    while (client_sock = Accept(socket_desc, (struct sockaddr *)&client, (socklen_t *)&c))
    {
        printf("Connection accepted %d ", client_sock);

        new_sock = malloc(1);
        *new_sock = client_sock;

        if (pthread_create(&sniffer_thread[clients], NULL, connection_handler, (void *)new_sock) < 0)
        {
            perror("could not create thread");
            return 1;
        }
        ++clients;
    }

    if (client_sock < 0)
    {
        perror("accept failed");
        return 1;
    }
    return 0;
}
/*
  This will handle connection for each client
  */
void *connection_handler(void *socket_desc)
{
    //Get the socket descriptor
    int sock = *(int *)socket_desc;
    int n;

    char sendBuff[4096], client_message[4096];

    while ((n = recv(sock, client_message, 4096, 0)) > 0)
    {

        send(sock, client_message, n, 0);
    }
    close(sock);

    if (n == 0)
    {
        printf("Client Disconnected %d", sock);
    }
    else
    {
        perror("recv failed");
    }
    return 0;
}