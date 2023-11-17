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

    int sockfd;              // socket descriptor
    struct sockaddr_in addr; // struct to hold server information
    char buffer[1024];       // buffer to hold data/message being exchanged
    socklen_t addr_size;     // size of the struct

    sockfd = socket(AF_INET, SOCK_DGRAM, 0); // create a UDP socket
    if (sockfd < 0)
    {
        perror("[-]Socket error");
        exit(1);
    }

    // server address configuration
    memset(&addr, '\0', sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(ip);

    // send data to server
    bzero(buffer, 1024);
    strcpy(buffer, "Hello, World!");
    // sendto(sockfd, buffer, 1024, 0, (struct sockaddr *)&addr, sizeof(addr));
    if(sendto(sockfd, buffer, 1024, 0, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        perror("[-]Sendto error");
        exit(1);
    }
    printf("[+]Data send: %s\n", buffer);

    // receive data from server
    bzero(buffer, 1024);
    addr_size = sizeof(addr);
    // recvfrom(sockfd, buffer, 1024, 0, (struct sockaddr *)&addr, &addr_size);
    if(recvfrom(sockfd, buffer, 1024, 0, (struct sockaddr *)&addr, &addr_size) < 0)
    {
        perror("[-]recvfrom error");
        exit(1);
    }
    printf("[+]Data recv: %s\n", buffer);

    return 0;
}