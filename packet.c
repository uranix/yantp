#include "packet.h"

#include <string.h>
#include <stdio.h>

void init_packet(struct packet *p) {
    memset(p, 0, sizeof(*p));
    p->magic = YANTPMAGIC;
}

void tag_client_send(struct packet *p) {
    clock_gettime(CLOCK_REALTIME, &p->client_send);
}

void tag_client_recv(struct packet *p) {
    clock_gettime(CLOCK_REALTIME, &p->client_recv);
}

void tag_server_send(struct packet *p) {
    clock_gettime(CLOCK_REALTIME, &p->server_send);
}

void tag_server_recv(struct packet *p) {
    clock_gettime(CLOCK_REALTIME, &p->server_recv);
}

bool verify_packet(const struct packet *p) {
    return p->magic == YANTPMAGIC;
}

static void print_time(const struct timespec *ts) {
    char buf[64];
    strftime(buf, 64, "%Y-%m-%d %H:%M:%S", localtime(&ts->tv_sec));
    printf("%s.%09ld\n", buf, ts->tv_nsec);
}

void print_packet(const struct packet *p) {
    printf("Verify: %s\n", verify_packet(p) ? "Valid" : "Invalid");
    printf("Client send time: ");
    print_time(&p->client_send);
    printf("Server recv time: ");
    print_time(&p->server_recv);
    printf("Server send time: ");
    print_time(&p->server_send);
    printf("Client recv time: ");
    print_time(&p->client_recv);
}

static double timediff(const struct timespec *beg, const struct timespec *end) {
    long sec = end->tv_sec - beg->tv_sec;
    long nsec = end->tv_nsec - beg->tv_nsec;
    return sec + 1e-9 * nsec;
}

double packet_roundtrip(const struct packet *p) {
    double elapsed = timediff(&p->client_send, &p->client_recv);
    double wait = timediff(&p->server_recv, &p->server_send);
    return elapsed - wait;
}

double packet_time_offset(const struct packet *p) {
    double send_diff = timediff(&p->client_send, &p->server_recv);
    double recv_diff = timediff(&p->server_send, &p->client_recv);

    return 0.5 * (send_diff - recv_diff);
}
