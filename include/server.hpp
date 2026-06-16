#pragma once

#include <cstdint>
#include <cstddef>
#include <optional>
#include <string>

struct Client{
        int fd;
        std::string in;
};

class TCPServer {
public:
    int start(uint16_t port);
    std::optional<std::string> recv_line(int fd);
    void stop(int client_fd);

private:
    //std::optional<std::string> handle_client(int fd);
    bool send_all(int fd, const char* data, size_t len);
    int server_fd_;
    static constexpr uint32_t MAX_MESSAGE_SIZE = 1024 * 1024;
};