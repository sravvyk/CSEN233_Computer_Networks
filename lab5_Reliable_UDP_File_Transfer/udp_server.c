//UDP Server - Side (with rdt3.0)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/select.h>

#define PLOSTMSG 5

typedef struct {
    int seq_ack;
    int len;
    int cksum;
} Header;

typedef struct {
    Header header;
    char data[10];
} Packet;

//getChecksum()
int getChecksum(Packet packet) {
    packet.header.cksum = 0;
    int checksum = 0;
    char *ptr = (char *)&packet;
    char *end = ptr + sizeof(Header) + packet.header.len;
    while (ptr < end) {
        checksum ^= *ptr++;
    }
    return checksum;
}

//print packet
void printPacket(Packet packet) {
    printf("Packet{ header: { seq_ack: %d, len: %d, cksum: %d }, data: \"",
    packet.header.seq_ack,
    packet.header.len,
    packet.header.cksum);
    fwrite(packet.data, (size_t)packet.header.len, 1, stdout);
    printf("\" }\n");
}

//serverSend()
void serverSend(int sockfd, const struct sockaddr *address, socklen_t addrlen, int seqnum) {
    // Simulating a chance that ACK gets lost
    if (rand() % PLOSTMSG == 0) {
        printf("Dropping ACK\n");
    }
    else {
        Packet packet;
        //prepare and send the ACK
        packet.header.seq_ack = seqnum;
        packet.header.len = 0;
        memset(packet.data, 0, sizeof(packet.data));
        packet.header.cksum = getChecksum(packet);

        //simulate a chance of sending a corrupted ACK
        if (rand() % PLOSTMSG == 0) {
            printf("Corrupting ACK\n");
            packet.header.cksum = 0;
        }

        sendto(sockfd, &packet, sizeof(packet), 0, address, addrlen);
        printf("Sent ACK %d, checksum %d\n", packet.header.seq_ack, packet.header.cksum);
    }
}

Packet serverReceive(int sockfd, struct sockaddr *address, socklen_t *addrlen, int seqnum) {
    Packet packet;
    while (1) {
        //Receive a packet from the client
        int bytes = recvfrom(sockfd, &packet, sizeof(packet), 0, address, addrlen);

        // validate the length of the packet
        if (bytes < 0) {
            perror("recvfrom failed");
            continue;
        }

        // print what was received
        printf("Received: ");
        printPacket(packet);

        //verify the checksum and the sequence number
        if (packet.header.cksum != getChecksum(packet)) {
            printf("Bad checksum, expected %d\n", getChecksum(packet));
            //send last ACK (resend previous ACK)
            serverSend(sockfd, address, *addrlen, (seqnum == 0) ? 1 : 0);
        } else if (packet.header.seq_ack != seqnum) {
            printf("Bad seqnum, expected %d\n", seqnum);
            //send last ACK (resend previous ACK)
            serverSend(sockfd, address, *addrlen, (seqnum == 0) ? 1 : 0);
        } else {
            printf("Good packet\n");
            //send ACK with the current sequence number
            serverSend(sockfd, address, *addrlen, seqnum);
            break;
        }
    }
    printf("\n");
    return packet;
}

int main(int argc, char *argv[]) {
    // check arguments
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <port> <outfile>\n", argv[0]);
        exit(1);
    }

    // seed the RNG
    srand((unsigned)time(NULL));

    // create a socket
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("socket failed");
        exit(1);
    }

    // initialize the server address structure
    struct sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(atoi(argv[1]));
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    // bind the socket
    if (bind(sockfd, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
        perror("bind failed");
        exit(1);
    }

    printf("Server waiting on port %s...\n", argv[1]);

    // open file using argv[2]
    int fp = open(argv[2], O_CREAT | O_RDWR | O_TRUNC, 0666);
    if (fp < 0) {
        perror("file failed to open\n");
        exit(1);
    }

    // get file contents from client and save it to the file
    int seqnum = 0;
    Packet packet;
    struct sockaddr_in clientAddr;
    socklen_t clientLen = sizeof(clientAddr);

    do {
        packet = serverReceive(sockfd, (struct sockaddr *)&clientAddr, &clientLen, seqnum);
        // write data to file if there is data
        if (packet.header.len > 0) {
            write(fp, packet.data, packet.header.len);
        }
        seqnum = (seqnum + 1) % 2;
    } while (packet.header.len > 0);

    printf("File transfer complete\n");

    //cleanup
    close(fp);
    close(sockfd);
    return 0;
}
