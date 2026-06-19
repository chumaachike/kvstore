#include <fstream>
#include <iostream>
#include <string>

#include "command_executor.hpp"
namespace persistence {
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

void rewrite_aof(KVStore& store, AOFLogger& logger, const std::string& snapshot_path, const std::string& aof_path) {
    logger.close();

    store.save_snapshot(snapshot_path);

    std::ofstream clear(aof_path, std::ios::trunc);
    if (!clear) {
        throw std::runtime_error("Could not clear AOF file");
    }

    logger.reopen(aof_path);
}
}