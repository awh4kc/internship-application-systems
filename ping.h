#include <iostream> // input output
#include <netdb.h> // getbyhostname
#include <string.h> // strcpy
#include <unistd.h> // sleep
#include <sys/socket.h> // inet_ntoa
#include <netinet/in.h> // inet_ntoa
#include <arpa/inet.h> // inet_ntoa
#include <netinet/ip_icmp.h> // icmphdr
#include <sys/types.h> // socket
#include <signal.h> // sigint
#include <bits/stdc++.h> // INT_MAX
#include <sys/time.h> // time

class Ping {
    public:
        Ping(std::string host): _host(host) {}
        void getip();
        void ping();
        void getsocket() {
            _sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
            if (_sockfd < 0) {
                fprintf(stderr, "Cannot create socket\n");
                _sockfd = -1;
            }
        }
        void printip() {
            puts(_ip_addr);
        }

    private:
        std::string _host;
        struct sockaddr_in _server;
        int _sockfd;
        int _count = 0;
        char* _ip_addr = (char*)malloc(sizeof(char)*45);
        unsigned short checksum(short* data, size_t bytes) {
            unsigned short ret;
            unsigned int sum = 0;
            unsigned short odd;
            while (bytes > 1) {
                sum += *data++;
                bytes -= 2;
            }
            if (bytes == 1) {
                *(unsigned short*)(&odd) = *(unsigned short*)data;
            }
            sum = (sum >> 16) + (sum & 0xFFFF);
            sum += (sum >> 16);
            ret = ~sum;
            return ret;
        };
};
