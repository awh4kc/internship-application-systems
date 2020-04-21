#include "ping.h"

int interrupt = 1;

void handler(int sig) {
    interrupt = 0;
}

void Ping::getip() {
    char host[256];
    struct addrinfo hints, *servinfo;
    int ret;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    if ((ret = getaddrinfo(_host.c_str(), NULL, &hints, &servinfo)) != 0) {
        fprintf(stdout, "getaddrinfo: %s", gai_strerror(ret));
        return;
    }

    getnameinfo(servinfo->ai_addr, servinfo->ai_addrlen, host, sizeof(host), NULL, 0, NI_NUMERICHOST);
    strcpy(_ip_addr, host);
    freeaddrinfo(servinfo);

    _server.sin_family = AF_UNSPEC;
    _server.sin_addr.s_addr = *(uint32_t*) servinfo->ai_addr;
    _server.sin_port = htons(0);
}

void Ping::ping() {
    struct sockaddr_in server;
    socklen_t server_addr_size;
    int transmitted = 0;
    int received = 0;
    struct timeval start;
    struct timeval stop;
    double min_val = INT_MAX;
    double max_val = 0.0;
    double average = 0.0;
    double std_dev = 0.0;
    double sum = 0.0;
    double var = 0.0;

    int ttl_val = 52;
    socklen_t ttl_val_size = sizeof(ttl_val);

    // Modify TTL in IP layer
    int stat = setsockopt(_sockfd, SOL_IP, IP_TTL, &ttl_val, ttl_val_size);
    if(stat != 0) {
        fprintf(stderr, "Failed to modify TTL in socket options\n");
        return;
    }

    struct timeval timeout;
    bzero(&timeout, sizeof(timeout));
    timeout.tv_sec = 2;

    // Modify timeout
    stat = setsockopt(_sockfd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
    if(stat != 0) {
        fprintf(stderr, "Failed to modify timeout in socket options\n");
        return;
    }

    // if ( fcntl(_sockfd, F_SETFL, O_NONBLOCK) != 0 )
        // perror("Request nonblocking I/O");

    int count = 0;
    gettimeofday(&start, NULL);
    while (interrupt) {
        count++;
        struct timeval start_ping;
        struct timeval stop_ping;
        struct icmphdr hdr;
        bzero(&hdr, sizeof(hdr));

        hdr.type = ICMP_ECHO;
        hdr.code = 0;
        hdr.un.echo.id = htons(getpid());
        hdr.un.echo.sequence = htons(count);
        hdr.checksum = 0;
        hdr.checksum = checksum((short*)&hdr, sizeof(hdr));

        sleep(1);

        gettimeofday(&start_ping, NULL);
        int packet_sent = 1;
        int st = sendto(_sockfd, &hdr, sizeof(hdr), 0, (struct sockaddr*)&_server, sizeof(_server));
        if (st < 0) {
            fprintf(stdout, "Cannot send packet.\n");
            packet_sent = 0;
        } else {
            transmitted++;
            fprintf(stdout, "sent: %d\n", st);
        }
        server_addr_size = sizeof(server);
        int status;
        int counter = 0;
        while (interrupt) {
            status = recvfrom(_sockfd, &hdr, sizeof(hdr), 0, (struct sockaddr*)&server, (socklen_t*)&server_addr_size);
            printf("counter: %d\n", counter++);
            fprintf(stdout, "error: %d\n", errno);
            if (!interrupt) {
                break;
            }
        }
        // int status = recvfrom(_sockfd, &hdr, sizeof(hdr), 0, (struct sockaddr*)&server, (socklen_t*)&server_addr_size);
        gettimeofday(&stop_ping, NULL);
        if (!interrupt) {
            break;
        }
        if (status <= 0) {
            fprintf(stdout, "error: %d\n", errno);
            fprintf(stdout, "status: %d\n", status);
            fprintf(stdout, "Didn't receive packet.\n");
        } else {
            if (packet_sent) {
                received++;
                double time = (((stop_ping.tv_sec - start_ping.tv_sec) * 1000000L + stop_ping.tv_usec) - start_ping.tv_usec)/1000.0;
                min_val = std::min(min_val, time);
                max_val = std::max(max_val, time);
                sum += time;
                average = sum / received;
                var += ((time - average) * (time - average));
                fprintf(stdout, "%d bytes from %s: icmp_seq=%d ttl=%d time=%0.1f ms\n", 64, _ip_addr, count, 52, time);
            }
        }
    }
    gettimeofday(&stop, NULL);
    double total_time = (((stop.tv_sec - start.tv_sec) * 1000000L + stop.tv_usec) - start.tv_usec)/1000.0;
    average = sum/received;
    std_dev = sqrt(var/received);

    if (!interrupt) {
        transmitted--;
    }

    fprintf(stdout, "\n--- %s ping statistics ---\n", _host.c_str());
    double packet_loss = 1.0 * (transmitted - received) / transmitted * 100;
    fprintf(stdout, "%d packets transmitted, %d received, %0.0f%% packet loss, time %0.0fms\n", transmitted, received, packet_loss, total_time);
    fprintf(stdout, "rtt min/avg/max/mdev = %0.3f/%0.3f/%0.3f/%0.3f ms\n", min_val, max_val, average, std_dev);

    if (close(_sockfd) != 0) {
        fprintf(stdout, "Failed to close socket\n");
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
    Ping ping(hostname);
    ping.getip();
    ping.getsocket();
    signal(SIGINT, handler);
    ping.ping();
}
