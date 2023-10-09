#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

typedef struct
{
    char last_input;
    char name;
    struct sockaddr_in client_addr;
} ClientInfo;

ClientInfo clients[2];

int server_sock;

void print_error(char *message)
{
    perror(message);
    close(server_sock);
    exit(1);
}

int main()
{
    char *ip = "127.0.0.1";
    int port = 5566;

    // int server_sock;
    struct sockaddr_in server_addr;
    socklen_t addr_size;

    server_sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (server_sock < 0)
        print_error("[-]Socket error");
    printf("[+]UDP server socket created.\n");

    memset(&server_addr, '\0', sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(ip);

    if (bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
        print_error("[-]Bind error");
    printf("[+]Bind to the port number: %d\n", port);

    int i;

    // Receive initial connection information from both clients
    for (i = 0; i < 2; i++)
    {
        addr_size = sizeof(struct sockaddr_in);
        char buffer[1024];
        bzero(buffer, 1024);

        ssize_t bytes_received = recvfrom(server_sock, buffer, sizeof(buffer), 0, (struct sockaddr *)&clients[i].client_addr, &addr_size);
        if (bytes_received <= 0)
            print_error("[-]Error receiving");

        // printf("%s\n", buffer);
        if (strcmp(buffer, "y\n") != 0)
        {
            printf("Player %c refused to play\n", 'A' + i);
            close(server_sock);
            exit(0);
        }

        clients[i].last_input = '\0';
        clients[i].name = 'A' + i;

        printf("[+]Player %c connected.\n", clients[i].name);
    }

    printf("\nGame begins\n\n");

    int flag;

    while (1)
    {
        flag = 0;
        // Receive input from both clients
        for (i = 0; i < 2; i++)
        {
            char buffer[1024];
            bzero(buffer, 1024);

            addr_size = sizeof(struct sockaddr_in);
            ssize_t bytes_received = recvfrom(server_sock, buffer, sizeof(buffer), 0, (struct sockaddr *)&clients[i].client_addr, &addr_size);
            if (bytes_received <= 0)
                print_error("[-]Error receiving");

            // Exit the loop if the user types any other key
            if (strcmp(buffer, "R\n") != 0 && strcmp(buffer, "P\n") != 0 && strcmp(buffer, "S\n") != 0)
            {
                flag = 1;
                break;
            }

            clients[i].last_input = buffer[0];
            printf("Player %c: %c\n", clients[i].name, clients[i].last_input);
        }

        char result[1024];
        bzero(result, 1024);

        if (flag)
        {
            snprintf(result, sizeof(result), "X");
            sendto(server_sock, result, strlen(result), 0, (struct sockaddr *)&clients[(i + 1) % 2].client_addr, sizeof(struct sockaddr_in));
            break;
        }

        // Determine the winner based on the game logic
        char a = clients[0].last_input;
        char b = clients[1].last_input;

        if (a == b)
            snprintf(result, sizeof(result), "Draw\n");
        else if ((a == 'R' && b == 'S') || (a == 'P' && b == 'R') || (a == 'S' && b == 'P'))
            snprintf(result, sizeof(result), "Player %c wins\n", clients[0].name);
        else
            snprintf(result, sizeof(result), "Player %c wins\n", clients[1].name);

        printf("Result: %s\n", result);

        // Send the result to both clients
        for (i = 0; i < 2; i++)
        {
            if (sendto(server_sock, result, strlen(result), 0, (struct sockaddr *)&clients[i].client_addr, sizeof(struct sockaddr_in)) < 0)
                print_error("[-]Error sending");
        }

        for (i = 0; i < 2; i++)
        {
            char buffer[1024];
            bzero(buffer, 1024);

            addr_size = sizeof(struct sockaddr_in);
            ssize_t bytes_received = recvfrom(server_sock, buffer, sizeof(buffer), 0, (struct sockaddr *)&clients[i].client_addr, &addr_size);
            if (bytes_received <= 0)
                print_error("[-]Error receiving");

            printf("%s", buffer);

            // Exit the loop if the user types any other key
            if (strcmp(buffer, "y\n") != 0)
            {
                flag = 1;
                break;
            }
        }
        // char result[1024];
        bzero(result, 1024);

        if (flag)
            snprintf(result, sizeof(result), "X");
        else
            snprintf(result, sizeof(result), "Continue");
        for (i = 0; i < 2; i++)
            sendto(server_sock, result, strlen(result), 0, (struct sockaddr *)&clients[i].client_addr, sizeof(struct sockaddr_in));
        if (flag)
            break;
        printf("\nNext round!\n");
    }

    printf("\nGame over\n");

    return 0;
}
