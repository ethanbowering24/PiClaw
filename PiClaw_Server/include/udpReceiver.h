#pragma once
#include "packet.h"

#include <sys/socket.h>   // socket(), sendto(), sockaddr_storage
#include <netdb.h>        // getaddrinfo(), freeaddrinfo()
#include <unistd.h>       // close()
#include <string.h>       // memcpy()
#include <string>         // std::string

class UdpReceiver {
public:
    UdpReceiver() : sockfd_(-1) {}

    ~UdpReceiver() { if (sockfd_ >= 0) close(sockfd_); }

    bool init(int port) {
        struct addrinfo hints{};
        hints.ai_family   = AF_UNSPEC;
        hints.ai_socktype = SOCK_DGRAM;
        hints.ai_flags    = AI_PASSIVE;

        struct addrinfo* res;
        if (getaddrinfo(nullptr, std::to_string(port).c_str(), &hints, &res) != 0)
            return false;

        sockfd_ = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
        if (sockfd_ < 0) {
            freeaddrinfo(res);
            return false;
        }

        if (bind(sockfd_, res->ai_addr, res->ai_addrlen) < 0) {
            freeaddrinfo(res);
            close(sockfd_);
            sockfd_ = -1;
            return false;
        }

        freeaddrinfo(res);
        return true;
    }

    bool receive(Packet& data, std::string* sender_ip = nullptr) {
        if (sockfd_ < 0) return false;

        struct sockaddr_storage sender_addr{};
        socklen_t addr_len = sizeof(sender_addr);

        ssize_t n = recvfrom(sockfd_, &data, sizeof(data), 0,
                             (struct sockaddr*)&sender_addr, &addr_len);
        if (n != sizeof(data)) return false;

        if (sender_ip) {
            char host[NI_MAXHOST];
            if (getnameinfo((struct sockaddr*)&sender_addr, addr_len,
                            host, sizeof(host), nullptr, 0, NI_NUMERICHOST) == 0)
                *sender_ip = host;
        }

        return true;
    }

private:
    int sockfd_;
};