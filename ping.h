#include <iostream> // input output
#include <netdb.h> // getbyhostname
#include <string.h> // strcpy

#include <sys/socket.h> // inet_ntoa
#include <netinet/in.h> // inet_ntoa
#include <arpa/inet.h> // inet_ntoa

#define TTL 52

class Ping {
    public:
        Ping(std::string host): _host(host) {}
        char* getip();
        void ping();

    private:
        std::string _host;
        struct sockaddr_in _server;
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
