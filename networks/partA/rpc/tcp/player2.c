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
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(ip);

    // initiating connection to server
    if (connect(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        perror("[-]Error connecting to server");
        close(sock);
        exit(1);
    }
    printf("Connected to the server.\n");

    while (1)
    {
        // sending data
        printf("Enter R for rock, P for paper, S for scissors(, any other key to exit): ");
        bzero(buffer, 1024);
        fgets(buffer, 1024, stdin);

        if (strcmp(buffer, "R\n") != 0 && strcmp(buffer, "P\n") != 0 && strcmp(buffer, "S\n") != 0)
            break;

        printf("Played: %s\n", buffer);
        if (send(sock, buffer, strlen(buffer), 0) < 0)
        {
            perror("[-]Error sending");
            close(sock);
            exit(1);
        }

        // receiving data
        bzero(buffer, 1024);
        if (recv(sock, buffer, sizeof(buffer), 0) < 0)
        {
            perror("[-]Error receiving");
            close(sock);
            exit(1);
        }
        if (strcmp(buffer, "X") == 0)
        {
            printf("Game over\n");
            break;
        }
        printf("Server: %s\n", buffer);

        printf("\nReady for the next round?\n");
        printf("Enter y to start, any other key to exit: ");
        bzero(buffer, 1024);
        fgets(buffer, 1024, stdin);

        if (send(sock, buffer, strlen(buffer), 0) < 0)
        {
            perror("[-]Error sending");
            close(sock);
            exit(1);
        }

        if (strcmp(buffer, "y\n") != 0)
        {
            printf("Exit.\n");
            break;
        }
        // receiving
        bzero(buffer, 1024);
        if (recv(sock, buffer, sizeof(buffer), 0) < 0)
        {
            perror("[-]Error receiving");
            close(sock);
            exit(1);
        }
        if (strcmp(buffer, "X") == 0)
        {
            printf("Game over\n");
            break;
        }
    }

    close(sock);
    printf("[-]Disconnected from server.\n");
    return 0;
}
