#pragma once

#include <string>
#include "store.hpp"


enum class CommandStatus {
    Continue,
    Quit
};

struct CommandResult {
    CommandStatus status;
    std::string response;
};

CommandResult execute_command(
    KVStore& store,
    const std::string& command_line
);