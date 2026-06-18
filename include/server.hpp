#pragma once

#include <cstdint>
#include <cstddef>
#include <optional>
#include <string>

#include "command_executor.hpp"

struct Client{
        int fd;
        std::string in;
};

class TCPServer {
public:
    void start(uint16_t port);
    explicit TCPServer(CommandExecutor);
    
private:
    int server_fd_;
    static constexpr uint32_t MAX_MESSAGE_SIZE = 1024 * 1024;
    std::optional<std::string> recv_line(int fd);
    bool send_all(int fd, const char* data, size_t len);
    int accept_client();
    void handle_client(int client_fd);
    CommandExecutor executor;
};