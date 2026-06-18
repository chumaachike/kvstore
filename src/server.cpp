#include <cstring>
#include <netdb.h>
#include <unistd.h>
#include <cerrno>
#include <sys/socket.h>
#include <stdexcept>
#include <iostream>
#include <thread>

#include "server.hpp"

TCPServer::TCPServer(CommandExecutor executor)
    : executor(std::move(executor)) {}

bool TCPServer::send_all(int fd, const char* data, size_t len){
    size_t total = 0;

    while (total < len){
        ssize_t sent = send(fd, data + total, len-total, 0);

        if (sent < 0 ){
            if (errno == EINTR) continue;
            return false;
        }
        if (sent == 0){
            return false;
        }
        total += sent;
    }
    return true;
}

std::optional<std::string> TCPServer::recv_line(int fd) {
    std::string line;
    char c;

    while (true) {
        ssize_t n = recv(fd, &c, 1, 0);

        if (n > 0) {
            if (c == '\n') {
                return line;
            }

            line.push_back(c);
        }
        else if (n == 0) {
            // client closed connection
            if (line.empty()) {
                return std::nullopt;
            }

            return line;
        }
        else {
            if (errno == EINTR) {
                continue;
            }

            return std::nullopt;
        }
    }
}

void TCPServer::start(uint16_t port){
    addrinfo hints{};
    addrinfo* res{};

    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    std::string port_str = std::to_string(port);
    int status = getaddrinfo(nullptr, port_str.c_str(), &hints, &res);

    if (status != 0){
        throw std::runtime_error(gai_strerror(status));
    }

    server_fd_ = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (server_fd_ == -1){
        freeaddrinfo(res);
        throw std::runtime_error(std::string("socket: ") + std::strerror(errno));
    }

    int yes = 1;
    if (setsockopt(server_fd_, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1) {
        close(server_fd_);
        freeaddrinfo(res);
        throw std::runtime_error(std::string("setsockopt: ") + std::strerror(errno));
    }

    if (bind(server_fd_, res->ai_addr, res->ai_addrlen) == -1){
        close(server_fd_);
        freeaddrinfo(res);
        throw std::runtime_error(std::string("bind: ") + std::strerror(errno));
    }

    freeaddrinfo(res);

    if (listen(server_fd_, SOMAXCONN) == -1){
        close(server_fd_);
        throw std::runtime_error(std::string("listen: ") + std::strerror(errno));
    }   
    while (true){
        int client_fd = accept_client();
        std::cout<<"Client connected ID: "<< client_fd << "\n";
        std::thread(&TCPServer::handle_client, this, client_fd).detach();
    }
}

int TCPServer::accept_client(){
    sockaddr_storage client_addr{};
    socklen_t client_len = sizeof(client_addr);

    int client_fd = accept(server_fd_, reinterpret_cast<sockaddr*>(&client_addr), &client_len);

    if (client_fd == -1){
        throw std::runtime_error(std::string("accept: ") + std::strerror(errno));        
    }
    return client_fd; 
}

void TCPServer::handle_client(int client_fd){
    while (true){
        auto command_line = recv_line(client_fd);

        if (!command_line){
            std::cerr << "Client disconnected\n";
            close(client_fd);
            return;
        }

        auto result = executor.execute(*command_line);

        if (!send_all(client_fd, result.response.c_str(), result.response.size())){
            std::cerr << "Failed to send response\n";
            close(client_fd);
            return;
        }
        
        if (result.status == CommandStatus::Quit){
            std::cout << "Client: " << client_fd << " quit\n";
            close(client_fd);
            return;
        }
    }
}