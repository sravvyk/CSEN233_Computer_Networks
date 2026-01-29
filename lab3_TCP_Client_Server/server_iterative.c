#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 10

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <port #>\n", argv[0]);
        exit(0);
    }

    int port = atoi(argv[1]);

    // socket and connection file descriptors
    int sockfd, connfd;

    // server and client address structures
    struct sockaddr_in serverAddr, clientAddr;
    socklen_t clientLen = sizeof(clientAddr);

    // buffers for receiving filename and sending file data
    char filename[256];
    char buffer[BUFFER_SIZE];

    // create TCP socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("socket failed");
        exit(1);
    }

    // setup server address structure
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(port);

    // bind socket to address
    if (bind(sockfd, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        perror("bind failed");
        exit(1);
    }

    // start listening, queue up to 5 connections
    if (listen(sockfd, 5) < 0) {
        perror("listen failed");
        exit(1);
    }

    printf("Server listening/waiting for client at port %d\n", port);

    while (1) {
        // accept incoming connection
        connfd = accept(sockfd, (struct sockaddr*)&clientAddr, &clientLen);
        if (connfd < 0) {
            perror("accept failed");
            continue;
        }

        printf("Connection Established with client IP: %s and Port: %d\n",
               inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));

        // receive filename from client (read until null terminator)
        memset(filename, 0, sizeof(filename));
        int i = 0;
        while (i < sizeof(filename) - 1) {
            int n = recv(connfd, &filename[i], 1, 0);
            if (n <= 0 || filename[i] == '\0')
                break;
            i++;
        }

        printf("Client requested file: %s\n", filename);

        // open the requested file
        FILE *fp = fopen(filename, "rb");
        if (fp == NULL) {
            perror("fopen failed");
            close(connfd);
            continue;
        }

        // read file and send to client in chunks
        int bytesRead;
        while ((bytesRead = fread(buffer, 1, BUFFER_SIZE, fp)) > 0) {
            send(connfd, buffer, bytesRead, 0);
        }

        printf("File transfer complete\n");

        fclose(fp);
        close(connfd);
    }

    close(sockfd);
    return 0;
}
