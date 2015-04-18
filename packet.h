#ifndef __PACKET_H__
#define __PACKET_H__

#include <time.h>
#include <stdint.h>
#include <stdbool.h>

#define YANTPMAGIC 0x56525350544e4159

struct packet {
    uint64_t magic;
    struct timespec client_send;
    struct timespec server_recv;
    struct timespec server_send;
    struct timespec client_recv;
};

bool verify_packet(const struct packet *p);
void init_packet(struct packet *p);
void tag_client_send(struct packet *p);
void tag_client_recv(struct packet *p);
void tag_server_send(struct packet *p);
void tag_server_recv(struct packet *p);

double packet_roundtrip(const struct packet *p);
double packet_time_offset(const struct packet *p);

void print_packet(const struct packet *p);

#endif
