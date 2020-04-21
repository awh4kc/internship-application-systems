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
#include <errno.h> // error

class Ping {
    public:
        Ping(char* host): _host(host) {}
        int getipaddr();
        int getsocket() {
            _sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
            if (_sockfd < 0) {
                fprintf(stdout, "Cannot create socket\n");
                fprintf(stdout, "You might try running this command as a superuser\n");
                return -1;
            }
            return 0;
        }
        char* getip() {
            return _ip_addr;
        }
        int ping();

    private:
        char* _host;
        struct sockaddr_in _server;
        int _sockfd;
        char* _ip_addr = (char *) malloc(sizeof(char)*45);
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
