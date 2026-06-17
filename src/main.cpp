#include <iostream>
#include <thread>
#include "store.hpp"
#include "server.hpp"
#include "server_loop.hpp"

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