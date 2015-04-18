#include "packet.h"

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <math.h>

#define PACKET_SEND_COUNT 30

struct measurement {
    double rtt;
    double offs;
};

static int cmp(const void *_a, const void *_b) {
    const struct measurement *a = _a;
    const struct measurement *b = _b;
    if (a->rtt < b->rtt) return -1;
    if (a->rtt > b->rtt) return 1;
    return 0;
}

int main(int argc, char **argv) {
    int sock;
    struct sockaddr_in saddr;
    struct packet p;
    int scnt, rcnt;
    int i, nsend = PACKET_SEND_COUNT;
    int nrecv = 0, nthrowaway, cnt;
    struct measurement meas[PACKET_SEND_COUNT];
    double mean, dev;

    struct timeval timeout;

    if (argc < 2) {
        fprintf(stderr, "USAGE: %s <server ip>\n\te.g. %s 127.0.0.1\n", argv[0], argv[0]);
        return EXIT_FAILURE;
    }

    if ((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
        perror("socket");
        return EXIT_FAILURE;
    }

    memset(&saddr, 0, sizeof(saddr));
    saddr.sin_family = AF_INET;
    saddr.sin_addr.s_addr = inet_addr(argv[1]);
    saddr.sin_port = htons(12345);

    if (connect(sock, (const struct sockaddr *)&saddr, sizeof(saddr)) < 0) {
        perror("connect");
        return EXIT_FAILURE;
    }

    timeout.tv_sec = 0;
    timeout.tv_usec = 100000;
    if (setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0) {
        perror("setsockopt timeout");
        return EXIT_FAILURE;
    }

    for (i = 0; i < nsend; i++) {
        init_packet(&p);
        tag_client_send(&p);
        scnt = send(sock, &p, sizeof(p), 0);
        if (scnt < 0) {
            perror("send");
            return EXIT_FAILURE;
        }
        rcnt = recv(sock, &p, sizeof(p), 0);
        if (rcnt < 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK)
                continue;
            perror("recv");
        }
        if (rcnt != sizeof(p))
            continue;
        tag_client_recv(&p);

        print_packet(&p);
        meas[nrecv].rtt  = packet_roundtrip(&p);
        meas[nrecv].offs = packet_time_offset(&p);
        nrecv++;
    }

    printf("%d packets received, %d packets sent\n", nrecv, nsend);

    qsort(meas, nrecv, sizeof(meas[0]), cmp);

    nthrowaway = nrecv / 10;
    for (i = 0; i < nrecv; i++) {
        if (i == nthrowaway)
            printf("-----------------------------------------\n");
        printf("%d. RTT = %.6lfms offset = %+.6lfms\n", i, 1e3 * meas[i].rtt, 1e3 * meas[i].offs);
        if (i == nrecv - nthrowaway - 1)
            printf("-----------------------------------------\n");
    }

    cnt = 0;
    mean = 0;
    dev = 0;
    for (i = nthrowaway; i < nrecv - nthrowaway; i++) {
        cnt++;
        mean += meas[i].offs;
        dev += meas[i].offs * meas[i].offs;
    }
    mean /= cnt;
    dev /= cnt;
    dev -= mean * mean;
    dev = sqrt(dev);

    printf("Time offset = %.6lf ms (std dev = %.6lf ms)\n", 1e3 * mean, 1e3 * dev);

    return EXIT_SUCCESS;
}
