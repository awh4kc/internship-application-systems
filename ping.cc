#include "ping.h"

int interrupt = 0;

char* Ping::getip() {
    char *ip_addr = (char *) malloc(sizeof(char)*1024);
    char host[256];
    struct addrinfo hints, *servinfo;
    int rv;

    memset(&hints, 0, sizeof hints);
    // hints.ai_family = AF_UNSPEC;
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    if ((rv = getaddrinfo(_host.c_str(), NULL, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s", gai_strerror(rv));
        char bad = '\0';
        char* ret = &bad;
        return ret;
    }

    getnameinfo(servinfo->ai_addr, servinfo->ai_addrlen, host, sizeof host, NULL, 0, NI_NUMERICHOST);
    strcpy(ip_addr, host);
    freeaddrinfo(servinfo);

    _server.sin_family = AF_UNSPEC;
    _server.sin_addr.s_addr = *(uint32_t*) servinfo->ai_addr;
    _server.sin_port = htons(12345);

    return ip_addr;
}

void Ping::ping() {

}

int main(int argc, char** argv) {
    Ping ping("google.com");
    char* ip = ping.getip();
    std::cout << ip << std::endl;
}
