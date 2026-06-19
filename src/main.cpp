#include <iostream>
#include "store.hpp"
#include "server.hpp"
#include "command_executor.hpp"
#include "logger.hpp"
#include "persistence.hpp"

int main() {
    KVStore kv_store;
    kv_store.load_snapshot("dump.kv");
    CommandExecutor replay_executor(kv_store, nullptr);
    replay_aof("appendonly.aof", replay_executor);

    try {
        AOFLogger* logger = new AOFLogger("appendonly.aof");

        CommandExecutor command_executor{kv_store, logger};
        TCPServer tcp_server{command_executor};

        tcp_server.start(8080);
        std::cout << "Server started...\n";
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << '\n';
        return 1;
    }

    return 0;
}