#pragma once

#include <cstdint>
#include <cstddef>
#include <optional>
#include <string>
#include <atomic>
#include <set>
#include <thread>

#include "command_executor.hpp"

class TCPServer {
public:
    explicit TCPServer(CommandExecutor);

    void start(uint16_t port);
    void stop();
    
private:
    int server_fd_{-1};
    std::atomic_bool running_{false};

    std::mutex clients_mutex_;
    std::set<int>client_fds_;
    std::vector <std::thread> client_threads_;

    static constexpr uint32_t MAX_MESSAGE_SIZE = 1024 * 1024;

    int accept_client();
    void handle_client(int client_fd);

    std::optional<std::string> recv_line(int fd);
    bool send_all(int fd, const char* data, size_t len);

    CommandExecutor executor;
};