#include <fstream>
#include <iostream>
#include <string>

#include "command_executor.hpp"

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