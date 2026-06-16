#include <iostream>
#include <thread>

#include "store.hpp"
#include "server.hpp"
#include "command_executor.hpp"

void handle_client(int client_fd, KVStore& kv_store, TCPServer& tcp_server) {
    while (true) {
        auto command_line = tcp_server.recv_line(client_fd);

        if (!command_line) {
            std::cout << "Client disconnected\n";
            tcp_server.stop(client_fd);
            return;
        }

        auto result = execute_command(kv_store, *command_line);

        if (!tcp_server.send_all(client_fd, result.response.c_str(), result.response.size())) {
            std::cout << "Failed to send response\n";
            tcp_server.stop(client_fd);
            return;
        }

        if (result.status == CommandStatus::Quit) {
            std::cout << "Client quit\n";
            tcp_server.stop(client_fd);
            return;
        }
    }
}

int main() {
    KVStore kv_store;
    TCPServer tcp_server;

    try{
        tcp_server.start(8080);
        std::cout << "Server started...\n";
    }catch(const std::exception& e){
        std::cerr << "Error: " << e.what() << '\n';
        return 1;
    }

    while (true) {
        int client_fd = tcp_server.accept_client();
        std::cout<<"Client connected ID: "<< client_fd << "\n";
        std::thread(handle_client, client_fd, std::ref(kv_store), std::ref(tcp_server)).detach();
    }    
    return 0;
}