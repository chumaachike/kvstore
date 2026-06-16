#include <cstring>
#include <netdb.h>
#include <string>
#include <stdexcept>
#include <unistd.h>
#include <iostream>
#include <cerrno>
#include <sys/socket.h>

#include "server.hpp"

void TCPServer::stop(int client_fd){
    close(client_fd);
}
bool TCPServer::send_all(int fd, const char* data, size_t len){
    size_t total = 0;

    while (total < len){
        ssize_t sent = send(fd, data + total, len-total, 0);

        if (sent <= 0 ){
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

int TCPServer::start(uint16_t port){
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

    std::cout << "Server listening on port "<< port << " ...\n";
    
    sockaddr_storage client_addr{};
    socklen_t client_len = sizeof(client_addr);

    int client_fd = accept(server_fd_, reinterpret_cast<sockaddr*>(&client_addr), &client_len);

    if (client_fd == -1){
        throw std::runtime_error(std::string("accept: ") + std::strerror(errno));        
    }

    std::cout << "Client connected \n";
    return client_fd;    
}


// std::optional<std::string> TCPServer::handle_client(int client_fd) {
//     while (true) {
//         auto line = recv_line(client_fd);

//         if (!line) {
//             return std::nullopt;
//         }

//        return line;
//     }
    
// }