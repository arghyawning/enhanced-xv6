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

    sock = socket(AF_INET, SOCK_DGRAM, 0); // create a UDP socket
    if (sock < 0)
    {
        perror("[-]Socket error");
        exit(1);
    }
    printf("[+]UDP client socket created.\n");

    // server address configuration
    memset(&addr, '\0', sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(ip);

    printf("Ready to play?\n");
    printf("Enter y to start, any other key to exit: ");
    bzero(buffer, 1024);
    fgets(buffer, 1024, stdin);
    if (sendto(sock, buffer, strlen(buffer), 0, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        perror("[-]Error sending");
        close(sock);
        exit(1);
    }
    // printf("Sent:%s\n", buffer);
    if (strcmp(buffer, "y\n") != 0)
    {
        printf("Exit.\n");
        close(sock);
        exit(1);
    }

    while (1)
    {
        // sending data
        printf("Enter R for rock, P for paper, S for scissors(, any other key to exit): ");
        bzero(buffer, 1024);
        fgets(buffer, 1024, stdin);

        // // Exit the loop if the user types 'q'
        // if (strcmp(buffer, "q\n") == 0)
        //     break;

        printf("Played: %s\n", buffer);
        if (sendto(sock, buffer, strlen(buffer), 0, (struct sockaddr *)&addr, sizeof(addr)) < 0)
        {
            perror("[-]Error sending");
            close(sock);
            exit(1);
        }

        // Exit the loop if the user types any other key
        if (strcmp(buffer, "R\n") != 0 && strcmp(buffer, "P\n") != 0 && strcmp(buffer, "S\n") != 0)
            break;

        // receiving data
        bzero(buffer, 1024);
        if (recvfrom(sock, buffer, sizeof(buffer), 0, NULL, NULL) < 0)
        {
            perror("[-]Error receiving");
            close(sock);
            exit(1);
        }
        if (strcmp(buffer, "X") == 0)
        {
            printf("Game over!\n");
            break;
        }
        printf("Server: %s\n", buffer);
    }

    close(sock);
    printf("[-]Disconnected from server.\n");
    return 0;
}
