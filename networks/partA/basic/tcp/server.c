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

    int server_sock, client_sock;                // socket descriptors
    struct sockaddr_in server_addr, client_addr; // structs to hold server and client information
    socklen_t addr_size;                         // size of each struct
    char buffer[1024];                           // buffer to hold data/message being exchanged
    int n;

    server_sock = socket(AF_INET, SOCK_STREAM, 0); // create a TCP socket
    if (server_sock < 0)
    {
        perror("[-]Socket error");
        exit(1);
    }
    printf("[+]TCP server socket created.\n");

    // server address configuration
    memset(&server_addr, '\0', sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(ip);

    // bind socket to the server address and port
    n = bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (n < 0)
    {
        perror("[-]Bind error");
        exit(1);
    }
    printf("[+]Bind to the port number: %d\n", port);

    // listen(server_sock, 5); // max 5 pending connections
    if (listen(server_sock, 5) < 0)
    {
        perror("[-]Listen error");
        close(server_sock);
        exit(1);
    }
    printf("Listening...\n");

    while (1) // infinite loop to handle multiple connections
    {
        // accepting connection from client
        addr_size = sizeof(client_addr);
        client_sock = accept(server_sock, (struct sockaddr *)&client_addr, &addr_size);
        if (client_sock < 0)
        {
            perror("[-]Error accepting");
            close(server_sock);
            exit(1);
        }
        printf("[+]Client connected.\n");

        // receiving data
        bzero(buffer, 1024);
        // recv(client_sock, buffer, sizeof(buffer), 0);
        if (recv(client_sock, buffer, sizeof(buffer), 0) < 0)
        {
            perror("[-]Error receiving");
            close(client_sock);
            exit(1);
        }
        printf("Client: %s\n", buffer);

        // sending data
        bzero(buffer, 1024);
        strcpy(buffer, "HI, THIS IS SERVER. HAVE A NICE DAY!!!");
        printf("Server: %s\n", buffer);
        // send(client_sock, buffer, strlen(buffer), 0);
        if (send(client_sock, buffer, strlen(buffer), 0) < 0)
        {
            perror("[-]Error sending");
            close(client_sock);
            exit(1);
        }

        close(client_sock);
        printf("[+]Client disconnected.\n\n");
    }

    return 0;
}