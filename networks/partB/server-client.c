#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

const int PORT = 8080;
const char *IP = "127.0.0.1";

#define CHUNK_SIZE 256
#define TIMEOUT 0.1
int *acks;

time_t start;

struct datapack
{
    int index;
    char data[CHUNK_SIZE];
    int numchunks;
};

struct arguments
{
    int sock;
    struct sockaddr_in server_address;
    int numchunks;
    struct datapack *chunks;
};

void *acknowledging(void *args)
{
    struct arguments *targs = (struct arguments *)args;

    int acksnum = 0;
    socklen_t address_size = sizeof(targs->server_address);
    while (acksnum < targs->numchunks)
    {
        // struct datapack dp;
        int index;
        if (recvfrom(targs->sock, &index, sizeof(int), 0, (struct sockaddr *)&targs->server_address, &address_size) < 0)
        // if (recvfrom(targs->sock, &index, sizeof(int), 0, NULL, NULL) < 0)
        {
            perror("receiving failed");
            exit(EXIT_FAILURE);
        }
        printf("[+]Ack recv: %d\n", index);
        acks[index] = 1;
        acksnum = 0;
        for (int i = 0; i < targs->numchunks; i++)
        {
            if (acks[i] == 1)
                acksnum++;
        }
    }
}

void *resending(void *args)
{
    struct arguments *targs = (struct arguments *)args;
    int acksnum = 0;
    socklen_t address_size = sizeof(targs->server_address);
    while (acksnum < targs->numchunks)
    {
        for (int i = 0; i < targs->numchunks; i++)
        {
            if (acks[i] == 0 && time(NULL) - start > TIMEOUT)
            {
                if (sendto(targs->sock, &targs->chunks[i], sizeof(targs->chunks[i]), 0, (struct sockaddr *)&targs->server_address, address_size) < 0)
                {
                    perror("sending failed");
                    exit(EXIT_FAILURE);
                }
            }
        }
        acksnum = 0;
        for (int i = 0; i < targs->numchunks; i++)
        {
            if (acks[i] == 1)
                acksnum++;
        }
    }
}

int chunking(char *data, struct datapack **dp)
{
    int numchunks = 1 + strlen(data) / CHUNK_SIZE;
    *dp = (struct datapack *)malloc(numchunks * sizeof(struct datapack));
    acks = (int *)calloc(numchunks, sizeof(int));

    int i, d = 0;
    for (i = 0; i < numchunks; i++)
    {
        (*dp)[i].index = i;
        (*dp)[i].numchunks = numchunks;
        strncpy((*dp)[i].data, data + d, CHUNK_SIZE);
        d += CHUNK_SIZE;
    }

    return numchunks;
}

int main()
{
    int sock;
    struct sockaddr_in server_address, client_address;
    int n;

    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0)
    {
        perror("[-]socket error");
        exit(1);
    }

    memset(&server_address, '\0', sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);
    server_address.sin_addr.s_addr = inet_addr(IP);

    n = bind(sock, (struct sockaddr *)&server_address, sizeof(server_address));
    if (n < 0)
    {
        perror("[-]bind error");
        exit(1);
    }

    char **message;

    struct datapack rdp;
    socklen_t address_size = sizeof(client_address);
    printf("[+]Server running...\n");
    int count = 0, chunksnum = 1;
    while (count < chunksnum)
    {
        n = recvfrom(sock, &rdp, sizeof(rdp), 0, (struct sockaddr *)&client_address, &address_size);
        if (n < 0)
        {
            perror("[-]recvfrom error ");
            exit(1);
        }
        // printf("[+]Data received in chunk %d: %s\n", rdp.index, rdp.data);
        chunksnum = rdp.numchunks;
        if (count == 0)
            message = (char **)malloc(rdp.numchunks * sizeof(char *));
        count++;

        int index;
        index = rdp.index;

        message[index] = (char *)malloc((CHUNK_SIZE + 1) * sizeof(char));
        strcpy(message[index], rdp.data);

        // Here, the server should send back an acknowledgment to the client
        n = sendto(sock, &index, sizeof(int), 0, (struct sockaddr *)&client_address, address_size);
        if (n < 0)
        {
            perror("[-]sendto error");
            exit(1);
        }
    }

    printf("Message received: \n");
    for (int i = 0; i < chunksnum; i++)
        printf("%s", message[i]);
    printf("\n");

    close(sock);

    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0)
    {
        perror("[-]socket error");
        exit(1);
    }

    printf("Enter message to send: ");
    // data input
    char *data = (char *)malloc(1073741824 * sizeof(char));
    if (data == NULL)
    {
        perror("malloc failed");
        exit(EXIT_FAILURE);
    }
    fgets(data, 1073741824, stdin);

    // chunking the data
    struct datapack *dp;
    int numchunks = chunking(data, &dp);
    acks = (int *)calloc(numchunks, sizeof(int));

    // arguments for threads
    struct arguments *targs = (struct arguments *)malloc(sizeof(struct arguments));
    targs->sock = sock;
    targs->server_address = server_address;
    targs->numchunks = numchunks;
    targs->chunks = dp;

    // creating the two threads
    pthread_t acknowledge, resend;
    pthread_create(&acknowledge, NULL, &acknowledging, targs);
    pthread_create(&resend, NULL, &resending, targs);

    // sending the data
    start = time(NULL);
    for (int i = 0; i < numchunks; i++)
    {
        if (sendto(sock, &dp[i], sizeof(dp[i]), 0, (struct sockaddr *)&server_address, sizeof(struct sockaddr_in)) < 0)
        {
            perror("sending failed");
            exit(EXIT_FAILURE);
        }
    }

    pthread_join(acknowledge, NULL);
    pthread_join(resend, NULL);

    close(sock);

    free(data);
    free(dp);
    free(acks);
    free(targs);

    return 0;
}
