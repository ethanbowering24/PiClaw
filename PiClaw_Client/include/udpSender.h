#pragma once

#include "packet.h"

#include <sys/socket.h>   // socket(), sendto(), sockaddr_storage
#include <netdb.h>        // getaddrinfo(), freeaddrinfo()
#include <unistd.h>       // close()
#include <string.h>       // memcpy()
#include <string>         // std::string
#include <stdexcept>      // std::runtime_error

class UdpSender {
public:
    UdpSender() : sockfd_(-1), addr_len_(0) {}

    ~UdpSender() { if (sockfd_ >= 0) close(sockfd_); }

    bool init(const std::string& host, const std::string& port) {
        struct addrinfo hints{};
        hints.ai_family   = AF_UNSPEC;
        hints.ai_socktype = SOCK_DGRAM;

        struct addrinfo* res;
        if (getaddrinfo(host.c_str(), port.c_str(), &hints, &res) != 0)
            return false;

        addr_len_ = res->ai_addrlen;
        memcpy(&addr_, res->ai_addr, addr_len_);
        freeaddrinfo(res);

        sockfd_ = socket(addr_.ss_family, SOCK_DGRAM, 0);
        if (sockfd_ < 0)
            return false;

        return true;
    }

    bool send(const Packet& data) {
        if (sockfd_ < 0) return false;
        ssize_t n = sendto(sockfd_, &data, sizeof(data), 0,
                           (struct sockaddr*)&addr_, addr_len_);
        return n == sizeof(data);
    }

private:
    int sockfd_;
    struct sockaddr_storage addr_;
    socklen_t addr_len_;
};