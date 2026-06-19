#include <fstream>
#include <iostream>
#include <string>

#include "command_executor.hpp"

namespace  {
void replay_aof(const std::string& path, CommandExecutor& executor) {
    std::ifstream in(path);

    if (!in) {
        return; 
    }

    std::string line;

    while (std::getline(in, line)) {
        if (line.empty()) {
            continue;
        }

        executor.execute(line);
    }
}

void rewrite_aof(KVStore& store,
                 AOFLogger& logger) {
    logger.close();

    store.save_snapshot("dump.kv");

    std::ofstream clear("appendonly.aof", std::ios::trunc);
    if (!clear) {
        throw std::runtime_error("Could not clear AOF file");
    }

    logger.reopen("appendonly.aof");
}
}
