#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

// Structure to hold client information
typedef struct
{
    int client_sock;
    char last_input;
    char name;
} ClientInfo;

ClientInfo clients[2]; // Array to store information for two clients

int main()
{
    char *ip = "127.0.0.1";
    int port = 5566;

    int server_sock;
    struct sockaddr_in server_addr;
    socklen_t addr_size;

    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock < 0)
    {
        perror("[-]Socket error");
        exit(1);
    }
    printf("[+]TCP server socket created.\n");

    // Enable SO_REUSEADDR option
    int opt = 1;
    if (setsockopt(server_sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
    {
        perror("[-]Setsockopt error");
        exit(1);
    }

    memset(&server_addr, '\0', sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(ip);

    if (bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("[-]Bind error");
        exit(1);
    }
    printf("[+]Bind to the port number: %d\n", port);

    if (listen(server_sock, 2) < 0)
    {
        perror("[-]Listen error");
        close(server_sock);
        exit(1);
    }
    printf("Waiting for players...\n");

    int i;

    // Accept two clients
    for (i = 0; i < 2; i++)
    {
        addr_size = sizeof(server_addr);
        int *client_sock = malloc(sizeof(int));
        *client_sock = accept(server_sock, (struct sockaddr *)&server_addr, &addr_size);

        if (*client_sock < 0)
        {
            perror("[-]Error accepting");
            close(server_sock);
            exit(1);
        }

        clients[i].client_sock = *client_sock;
        clients[i].last_input = '\0';
        clients[i].name = 'A' + i;

        printf("[+]Player %c connected.\n", clients[i].name);
    }

    int flag;
    while (1)
    {
        flag = 0;
        // Receive input from both clients
        for (i = 0; i < 2; i++)
        {
            char buffer[1024];
            bzero(buffer, 1024);

            ssize_t bytes_received = recv(clients[i].client_sock, buffer, sizeof(buffer), 0);
            if (bytes_received <= 0)
                perror("[-]Error receiving");

            // Exit the loop if the user types any other key
            if (strcmp(buffer, "R\n") != 0 && strcmp(buffer, "P\n") != 0 && strcmp(buffer, "S\n") != 0)
            {
                flag = 1;
                break;
            }

            clients[i].last_input = buffer[0];
            printf("Player %c: %c\n", clients[i].name, clients[i].last_input);
        }

        // Determine the winner based on the game logic
        char result[1024];
        bzero(result, 1024);

        if (flag)
        {
            snprintf(result, sizeof(result), "X");
            send(clients[(i + 1) % 2].client_sock, result, strlen(result), 0);
            break;
        }

        char a = clients[0].last_input;
        char b = clients[1].last_input;

        if (a == b)
            snprintf(result, sizeof(result), "Draw\n");
        else if ((a == 'R' && b == 'S') || (a == 'P' && b == 'R') || (a == 'S' && b == 'P'))
            snprintf(result, sizeof(result), "Player %c wins\n", clients[0].name);
        else
            snprintf(result, sizeof(result), "Player %c wins\n", clients[1].name);

        printf("Result: %s", result);

        // Send the result to both clients
        for (i = 0; i < 2; i++)
        {
            if (send(clients[i].client_sock, result, strlen(result), 0) < 0)
            {
                perror("[-]Error sending");
                close(clients[i].client_sock);
                exit(1);
            }
        }

        for (i = 0; i < 2; i++)
        {
            char buffer[1024];
            bzero(buffer, 1024);

            ssize_t bytes_received = recv(clients[i].client_sock, buffer, sizeof(buffer), 0);
            if (bytes_received <= 0)
                perror("[-]Error receiving");

            if (strcmp(buffer, "y\n") != 0)
            {
                flag = 1;
                break;
            }
        }
        bzero(result, 1024);
        if (flag)
            snprintf(result, sizeof(result), "X");
        else
            snprintf(result, sizeof(result), "Continue");
        for (i = 0; i < 2; i++)
            send(clients[i].client_sock, result, strlen(result), 0);
        if (flag)
            break;
        printf("\nNext round!\n");
    }
    printf("\nGame over\n");

    return 0;
}
