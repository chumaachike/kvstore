#pragma once

#include <string>
#include "store.hpp"
#include "parser.hpp"


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

    CommandResult execute(const std::string& input);

private:
    KVStore& store;
    Parser parser;
    std::string handle_set(const std::vector<std::string>& args);
};