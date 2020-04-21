#include "ping.h"

bool interrupt = true;

void handler(int sig) {
    interrupt = !sig;
}

void Ping::getipaddr() {
    struct hostent *host;
    host = gethostbyname(_host);
    if (host == NULL) {
        fprintf(stderr, "Could not get ip address\n");
    }
    strcpy(_ip_addr, inet_ntoa(*(struct in_addr *)host->h_addr));
    _server.sin_family = host->h_addrtype;
    _server.sin_addr.s_addr = *(uint32_t*) host->h_addr;
    _server.sin_port = htons(0);
}

void Ping::ping() {
    struct sockaddr_in server;
    int sent_total = 0;
    int received_total = 0;
    struct timeval start;
    struct timeval stop;
    double min_val = INT_MAX;
    double max_val = 0.0;
    double average = 0.0;
    double std_dev = 0.0;
    double sum = 0.0;
    double var = 0.0;
    int count = 0;
    bool sent;

    gettimeofday(&start, NULL);
    while (interrupt) {
        count += 1;
        struct timeval start_ping;
        struct timeval stop_ping;
        struct icmphdr icmp_header;
        memset(&icmp_header, 0, sizeof(icmp_header));

        icmp_header.type = ICMP_ECHO;
        icmp_header.code = 0;
        icmp_header.un.echo.id = htons(getpid());
        icmp_header.un.echo.sequence = htons(count);
        icmp_header.checksum = 0;
        icmp_header.checksum = checksum((short*)&icmp_header, sizeof(icmp_header));
        sleep(1);

        gettimeofday(&start_ping, NULL);
        if (sendto(_sockfd, &icmp_header, sizeof(icmp_header), 0, (struct sockaddr*)&_server, sizeof(_server)) < 0) {
            fprintf(stderr, "Cannot send packet.\n");
            sent = false;
        } else {
            sent_total += 1;
            sent = true;
        }
        if (!interrupt) {
            break;
        }
        socklen_t size = sizeof(server);
        if (recvfrom(_sockfd, &icmp_header, sizeof(icmp_header), 0, (struct sockaddr*)&server, &size) <= 0) {
            fprintf(stderr, "Didn't receive packet.\n");
        } else {
            gettimeofday(&stop_ping, NULL);
            if (sent) {
                received_total += 1;
                double time = (((stop_ping.tv_sec - start_ping.tv_sec) * 1000000L + stop_ping.tv_usec) - start_ping.tv_usec)/1000.0;
                min_val = std::min(min_val, time);
                max_val = std::max(max_val, time);
                sum += time;
                average = sum / received_total;
                var += ((time - average) * (time - average));
                fprintf(stdout, "%d bytes from %s: icmp_seq=%d ttl=%d time=%0.1f ms\n", 64, _ip_addr, count, 52, time);
            }
        }
    }
    gettimeofday(&stop, NULL);
    double total_time = (((stop.tv_sec - start.tv_sec) * 1000000L + stop.tv_usec) - start.tv_usec)/1000.0;
    average = sum/received_total;
    std_dev = sqrt(var/received_total);

    if (!interrupt) {
        sent_total--;
    }

    fprintf(stdout, "\n--- %s ping statistics ---\n", _host);
    double packet_loss = 1.0 * (sent_total - received_total) / sent_total * 100;
    fprintf(stdout, "%d packets sent_total, %d received, %0.0f%% packet loss, time %0.0fms\n", sent_total, received_total, packet_loss, total_time);
    fprintf(stdout, "rtt min/avg/max/mdev = %0.3f/%0.3f/%0.3f/%0.3f ms\n", min_val, max_val, average, std_dev);

    if (close(_sockfd) != 0) {
        fprintf(stderr, "Failed to close socket\n");
        return;
    }
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
    Ping pong(hostname);
    pong.getsocket();
    pong.getipaddr();
    char* ip_addr = pong.getip();
    fprintf(stdout, "PING: %s (%s) %d bytes of data.\n", hostname, ip_addr, 64);
    signal(SIGINT, handler);
    pong.ping();
    return 0;
}
