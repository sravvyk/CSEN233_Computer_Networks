#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 10

int main(int argc, char *argv[]) {
    if (argc != 5) {
        printf("Usage: %s <server IP> <port> <remote file> <local file>\n", argv[0]);
        exit(0);
    }

    char *serverIP = argv[1];
    int port = atoi(argv[2]);
    char *remoteFile = argv[3];
    char *localFile = argv[4];

    int sockfd;
    struct sockaddr_in serverAddr;
    char buffer[BUFFER_SIZE];

    // create TCP socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("socket failed");
        exit(1);
    }

    // setup server address
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    inet_pton(AF_INET, serverIP, &serverAddr.sin_addr);

    // connect to server
    printf("Connecting to server %s:%d...\n", serverIP, port);
    if (connect(sockfd, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        perror("connect failed");
        exit(1);
    }
    printf("Connected!\n");

    // send filename to server (with null terminator)
    printf("Requesting file: %s\n", remoteFile);
    send(sockfd, remoteFile, strlen(remoteFile) + 1, 0);

    // open local file to write
    FILE *fp = fopen(localFile, "wb");
    if (fp == NULL) {
        perror("fopen failed");
        close(sockfd);
        exit(1);
    }

    // receive file data in chunks
    int bytesReceived;
    int totalBytes = 0;

    while ((bytesReceived = recv(sockfd, buffer, BUFFER_SIZE, 0)) > 0) {
        fwrite(buffer, 1, bytesReceived, fp);
        totalBytes += bytesReceived;
    }

    printf("File transfer complete. Received %d bytes.\n", totalBytes);
    printf("Saved to: %s\n", localFile);

    fclose(fp);
    close(sockfd);

    return 0;
}
