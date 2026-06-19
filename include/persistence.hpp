#pragma once
#include <string>
#include "command_executor.hpp"
void replay_aof(const std::string& path, CommandExecutor& executor);