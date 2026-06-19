#include <iostream>
#include <csignal>
#include <atomic>
#include <thread>
#include <chrono>

#include "store.hpp"
#include "server.hpp"
#include "command_executor.hpp"
#include "logger.hpp"
#include "persistence.hpp"

std::atomic_bool shutdown_requested{false};

void handle_signal(int) {
    shutdown_requested = true;
}

int main() {
    std::signal(SIGINT, handle_signal);
    std::signal(SIGTERM, handle_signal);

    try {
        KVStore kv_store;

        kv_store.load_snapshot("dump.kv");

        CommandExecutor replay_executor(kv_store, nullptr);
        persistence::replay_aof("appendonly.aof", replay_executor);

        AOFLogger logger("appendonly.aof");

        CommandExecutor command_executor(kv_store, &logger);
        TCPServer tcp_server(command_executor);

        std::thread server_thread([&tcp_server] {
            tcp_server.start(8080);
        });

        std::cout << "Server started on port 8080\n";

        while (!shutdown_requested) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }

        std::cout << "Shutdown requested\n";

        tcp_server.stop();

        if (server_thread.joinable()) {
            server_thread.join();
        }

        persistence::rewrite_aof(
            kv_store,
            logger,
            "dump.kv",
            "appendonly.aof"
        );

        std::cout << "Server shutdown complete\n";
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << '\n';
        return 1;
    }

    return 0;
}