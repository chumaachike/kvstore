#pragma once
#include <string>
#include "command_executor.hpp"
namespace persistence {
void replay_aof(const std::string& path, CommandExecutor& executor);
void rewrite_aof(KVStore& store, AOFLogger& logger, const std::string& snapshot_path, const std::string& aof_path);
}
