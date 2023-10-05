#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        printf("Usage: %s <port>\n", argv[0]);
        exit(0);
    }

    // IP and port number of server
    char *ip = "127.0.0.1";
    int port = atoi(argv[1]);

    int sockfd;                                  // socket descriptor
    struct sockaddr_in server_addr, client_addr; // structs to hold server and client information
    char buffer[1024];                           // buffer to hold data/message being exchanged
    socklen_t addr_size;                         // size of each struct
    int n;

    sockfd = socket(AF_INET, SOCK_DGRAM, 0); // create a UDP socket
    if (sockfd < 0)
    {
        perror("[-]socket error");
        exit(1);
    }

    // server address configuration
    memset(&server_addr, '\0', sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(ip);

    // bind socket to the server address and port
    n = bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (n < 0)
    {
        perror("[-]bind error");
        exit(1);
    }

    // receive data from client
    bzero(buffer, 1024);
    addr_size = sizeof(client_addr);
    // recvfrom(sockfd, buffer, 1024, 0, (struct sockaddr *)&client_addr, &addr_size);
    if (recvfrom(sockfd, buffer, 1024, 0, (struct sockaddr *)&client_addr, &addr_size) < 0)
    {
        perror("[-]recvfrom error");
        exit(1);
    }
    printf("[+]Data recv: %s\n", buffer);

    // send data to client
    bzero(buffer, 1024);
    strcpy(buffer, "Welcome to the UDP Server.");
    // sendto(sockfd, buffer, 1024, 0, (struct sockaddr *)&client_addr, sizeof(client_addr));
    if (sendto(sockfd, buffer, 1024, 0, (struct sockaddr *)&client_addr, sizeof(client_addr)) < 0)
    {
        perror("[-]sendto error");
        exit(1);
    }
    printf("[+]Data send: %s\n", buffer);

    return 0;
}