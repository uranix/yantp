#include "packet.h"

int main() {
    struct packet p;
    init_packet(&p);
    print_packet(&p);
    tag_client_send(&p);
    print_packet(&p);
    tag_server_recv(&p);
    print_packet(&p);
    tag_server_send(&p);
    print_packet(&p);
    tag_client_recv(&p);
    print_packet(&p);
    return 0;
}
