#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

int main()
{
    // IP and port number of server
    char *ip = "127.0.0.1";
    int port = 5566;

    int sock;                // socket descriptor
    struct sockaddr_in addr; // struct to hold server information
    socklen_t addr_size;     // size of the struct
    char buffer[1024];       // buffer to hold data/message being exchanged
    int n;

    sock = socket(AF_INET, SOCK_STREAM, 0); // create a TCP socket
    if (sock < 0)
    {
        perror("[-]Socket error");
        exit(1);
    }
    printf("[+]TCP server socket created.\n");

    // server address configuration
    memset(&addr, '\0', sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = port;
    addr.sin_addr.s_addr = inet_addr(ip);

    // initiating connection to server
    // connect(sock, (struct sockaddr *)&addr, sizeof(addr));
    if (connect(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        perror("[-]Error connecting to server");
        close(sock);
        exit(1);
    }
    printf("Connected to the server.\n");

    // sending data
    bzero(buffer, 1024);
    strcpy(buffer, "HELLO, THIS IS CLIENT!.");
    printf("Client: %s\n", buffer);
    // send(sock, buffer, strlen(buffer), 0);
    if (send(sock, buffer, strlen(buffer), 0) < 0)
    {
        perror("[-]Error sending");
        close(sock);
        exit(1);
    }

    // receiving data
    bzero(buffer, 1024);
    // recv(sock, buffer, sizeof(buffer), 0);
    if (recv(sock, buffer, sizeof(buffer), 0) < 0)
    {
        perror("[-]Error receiving");
        close(sock);
        exit(1);
    }
    printf("Server: %s\n", buffer);

    close(sock);
    printf("Disconnected from the server.\n");

    return 0;
}