#pragma once

#include <stdint.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>

class TCPSocket {
public:
    TCPSocket() = default;
    TCPSocket(const TCPSocket&) = delete;
    TCPSocket& operator=(const TCPSocket&) = delete;

    ~TCPSocket() { unInit(); }

public:
    bool init() {
        fd_ = socket(AF_INET, SOCK_STREAM, 0);
        if (fd_ < 0) {
            log_err(kESocket, "socket failed. errno[%d]", errno);
            return false;
        }

        return true;
    }

    bool bind(const uint16_t port) {
        struct sockaddr_in addr;
        memset(&addr, 0, sizeof(addr));
        addr.sin_family = AF_INET;
        addr.sin_port   = htons(port);
        addr.sin_addr.s_addr = htonl(INADDR_ANY);

        if (::bind(fd_, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
            log_err(kEBind, "bind failed. errno[%d]", errno);
            return false;
        }

        return true;
    }

    bool listen(const uint32_t backlog) {
        if (::listen(fd_, backlog) < 0) {
            log_err(kEListen, "listen failed. errno[%d]", errno);
            return false;
        }

        return true;
    }

    int accept(struct sockaddr_in* addr) {
        socklen_t len = sizeof(struct sockaddr_in);
        return ::accept(fd_, (struct sockaddr*)addr, &len);
    }

    bool connect(const char* ip, const uint16_t port) {
        struct sockaddr_in addr;
        memset(&addr, 0, sizeof(addr));
        addr.sin_family = AF_INET;
        addr.sin_port   = htons(port);
        if (inet_pton(AF_INET, ip, &addr.sin_addr) <= 0) {
            log_err(kEConnect, "inet_pton failed. errno[%d]", errno);
            return false;
        }

        if (::connect(fd_, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
            log_err(kEConnect, "connect failed. errno[%d]", errno);
            return false;
        }

        return true;
    }

    bool setNonBlock() {
        int flags = fcntl(fd_, F_GETFL, 0);
        if (flags < 0) {
            log_err(kEFCntl, "fcntl F_GETFL failed. errno[%d]", errno);
            return false;
        }

        if (fcntl(fd_, F_SETFL, flags | O_NONBLOCK) < 0) {
            log_err(kEFCntl, "fcntl F_SETFL failed. errno[%d]", errno);
            return false;
        }
    }

    bool setReuseAddr() {
        int opt = 1;
        if (setsockopt(fd_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
            log_err(kESetSockOpt, "setsockopt SO_REUSEADDR failed. errno[%d]", errno);
            return false;
        }

        return true;
    }

    bool setNoDelay() {
        int opt = 1;
        if (setsockopt(fd_, IPPROTO_TCP, TCP_NODELAY, &opt, sizeof(opt)) < 0) {
            log_err(kESetSockOpt, "setsockopt TCP_NODELAY failed. errno[%d]", errno);
            return false;
        }

        return true;
    }

    bool setKeepAlive() {
        int opt = 1;
        if (setsockopt(fd_, SOL_SOCKET, SO_KEEPALIVE, &opt, sizeof(opt)) < 0) {
            log_err(kESetSockOpt, "setsockopt SO_KEEPALIVE failed. errno[%d]", errno);
            return false;
        }

        return true;
    }

    bool setSendBuf(const uint32_t size) {
        if (setsockopt(fd_, SOL_SOCKET, SO_SNDBUF, &size, sizeof(size)) < 0) {
            log_err(kESetSockOpt, "setsockopt SO_SNDBUF failed. errno[%d]", errno);
            return false;
        }

        return true;
    }

    bool setRecvBuf(const uint32_t size) {
        if (setsockopt(fd_, SOL_SOCKET, SO_RCVBUF, &size, sizeof(size)) < 0) {
            log_err(kESetSockOpt, "setsockopt SO_RCVBUF failed. errno[%d]", errno);
            return false;
        }

        return true;
    }

    bool setSendTimeout(const uint32_t timeout) {
        struct timeval tv;
        tv.tv_sec  = timeout / 1000;
        tv.tv_usec = (timeout % 1000) * 1000;
        if (setsockopt(fd_, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv)) < 0) {
            log_err(kESetSockOpt, "setsockopt SO_SNDTIMEO failed. errno[%d]", errno);
            return false;
        }

        return true;
    }

    bool setRecvTimeout(const uint32_t timeout) {
        struct timeval tv;
        tv.tv_sec  = timeout / 1000;
        tv.tv_usec = (timeout % 1000) * 1000;
        if (setsockopt(fd_, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0) {
            log_err(kESetSockOpt, "setsockopt SO_RCVTIMEO failed. errno[%d]", errno);
            return false;
        }

        return true;
    }

    bool setLinger(const uint32_t timeout) {
        struct linger l;
        l.l_onoff  = 1;
        l.l_linger = timeout;
        if (setsockopt(fd_, SOL_SOCKET, SO_LINGER, &l, sizeof(l)) < 0) {
            log_err(kESetSockOpt, "setsockopt SO_LINGER failed. errno[%d]", errno);
            return false;
        }

        return true;
    }

    bool setNoSigPipe() {
        int opt = 1;
        if (setsockopt(fd_, SOL_SOCKET, SO_NOSIGPIPE, &opt, sizeof(opt)) < 0) {
            log_err(kESetSockOpt, "setsockopt SO_NOSIGPIPE failed. errno[%d]", errno);
            return false;
        }

        return true;
    }

    bool setNoSigPipe(const int fd) {
        int opt = 1;
        if (setsockopt(fd, SOL_SOCKET, SO_NOSIGPIPE, &opt, sizeof(opt)) < 0) {
            log_err(kESetSockOpt, "setsockopt SO_NOSIGPIPE failed. errno[%d]", errno);
            return false;
        }

        return true;
    }

private:
    void unInit() {
        if (fd_ >= 0) {
            close(fd_);
            fd_ = -1;
        }
    }

private:    
    int fd_ = -1;
};