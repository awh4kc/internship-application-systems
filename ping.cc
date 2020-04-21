#include "ping.h"

int interrupt = 0;

void Ping::getip() {
    char host[256];
    struct addrinfo hints, *servinfo;
    int ret;

    memset(&hints, 0, sizeof hints);
    // hints.ai_family = AF_UNSPEC;
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    if ((ret = getaddrinfo(_host.c_str(), NULL, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s", gai_strerror(ret));
        return;
    }

    getnameinfo(servinfo->ai_addr, servinfo->ai_addrlen, host, sizeof host, NULL, 0, NI_NUMERICHOST);
    strcpy(_ip_addr, host);
    freeaddrinfo(servinfo);

    _server.sin_family = AF_UNSPEC;
    _server.sin_addr.s_addr = *(uint32_t*) servinfo->ai_addr;
    _server.sin_port = htons(12345);
}

void Ping::ping() {

}

void parse_command(int argc, char** argv, char** hostname) {
    for (int i = 0; i < argc; ++i) {
        if (argv[i][0] == '-') {
            puts(argv[i]);
        } else {
            *hostname = argv[i];
        }
    }
}

int main(int argc, char** argv) {
    char* hostname = NULL;
    parse_command(argc, argv, &hostname);
    Ping ping(hostname);
    ping.getip();
    ping.printip();
    ping.ping();
}
