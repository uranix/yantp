#include "packet.h"

#include <stdio.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char **argv) {
    int sock;
    struct sockaddr_in saddr, caddr;
    struct packet p;
    ssize_t recvsize;
    socklen_t fromlen;

    memset(&saddr, 0, sizeof(saddr));
    saddr.sin_family = AF_INET;
    saddr.sin_addr.s_addr = htonl(INADDR_ANY);
    saddr.sin_port = htons(12345);

    if ((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
        perror("socket");
        return EXIT_FAILURE;
    }

    if (bind(sock, (struct sockaddr *)&saddr, sizeof(saddr)) < 0) {
        perror("bind");
        close(sock);
        return EXIT_FAILURE;
    }

    char *straddr = inet_ntoa(saddr.sin_addr);
    printf("Running %s on %s:%d\n", argv[0], straddr
            , ntohs(saddr.sin_port));

    while(1) {
        fromlen = sizeof(saddr);
        recvsize = recvfrom(sock, &p, sizeof(p), 0,
                (struct sockaddr *)&caddr, &fromlen);
        if (recvsize < 0) {
            perror("recvfrom");
            return EXIT_FAILURE;
        }

        if (recvsize != sizeof(p))
            continue;

        tag_server_recv(&p);


        if (verify_packet(&p)) {
            printf("Recv packet from %s:%d\n",
                    inet_ntoa(caddr.sin_addr), ntohs(caddr.sin_port));
            print_packet(&p);
            if ((rand() & 3) == 0) {
                printf("Packet dropped\n");
                continue;
            }
            tag_server_send(&p);
            sendto(sock, &p, sizeof(p), 0,
                (const struct sockaddr *)&caddr, fromlen);
        }
    }
}
