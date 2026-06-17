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

class CommandExecutor {
public:
    explicit CommandExecutor(KVStore& store);

    std::string execute(const std::string& input);

private:
    KVStore& store;
};