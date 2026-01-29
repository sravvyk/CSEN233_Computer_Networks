#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

// max number of clients/threads
#define N 100
#define BUFFER_SIZE 10

int threadCount = 0;
pthread_t clients[N];

// client address (global so thread can access it)
struct sockaddr_in clientAddr;

// thread function to handle file transfer
void* connectionHandler(void* arg) {
    int connfd = (int)(intptr_t)arg;

    char filename[256];
    char buffer[BUFFER_SIZE];

    // receive filename from client
    memset(filename, 0, sizeof(filename));
    int i = 0;
    while (i < sizeof(filename) - 1) {
        int n = recv(connfd, &filename[i], 1, 0);
        if (n <= 0 || filename[i] == '\0')
            break;
        i++;
    }

    printf("Client requested file: %s\n", filename);

    // open file
    FILE *fp = fopen(filename, "rb");
    if (fp == NULL) {
        perror("fopen failed");
        close(connfd);
        pthread_exit(0);
    }

    // read and send file in chunks
    int bytesRead;
    while ((bytesRead = fread(buffer, 1, BUFFER_SIZE, fp)) > 0) {
        send(connfd, buffer, bytesRead, 0);
    }

    printf("File transfer complete\n");

    fclose(fp);
    close(connfd);

    pthread_exit(0);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <port #>\n", argv[0]);
        exit(0);
    }

    int port = atoi(argv[1]);

    int sockfd, connfd;
    struct sockaddr_in serverAddr;
    socklen_t clientLen = sizeof(clientAddr);

    // create TCP socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("socket failed");
        exit(1);
    }

    // setup server address
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(port);

    // bind
    if (bind(sockfd, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        perror("bind failed");
        exit(1);
    }

    // listen
    if (listen(sockfd, 5) < 0) {
        perror("listen failed");
        exit(1);
    }

    printf("Server listening/waiting for client at port %d\n", port);

    while (1) {
        // accept connection
        connfd = accept(sockfd, (struct sockaddr*)&clientAddr, &clientLen);
        if (connfd < 0) {
            perror("accept failed");
            continue;
        }

        printf("Connection Established with client IP: %s and Port: %d\n",
               inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));

        // create thread to handle this client
        if (pthread_create(&clients[threadCount], NULL, connectionHandler, (void*)(intptr_t)connfd) < 0) {
            perror("pthread_create failed");
            exit(0);
        }

        printf("Thread %d has been created to service client request\n", ++threadCount);
    }

    // wait for all threads (not reached due to infinite loop)
    for (int i = 0; i < threadCount; i++) {
        pthread_join(clients[i], NULL);
    }

    return 0;
}
