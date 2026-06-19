#pragma once

#include <string>
#include "store.hpp"
#include "parser.hpp"
#include "logger.hpp"


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
    CommandExecutor(KVStore& store, AOFLogger* logger);

    CommandResult execute(const std::string& input);

private:
    KVStore& store;
    Parser parser;
    AOFLogger* logger;
    std::string handle_set(const std::vector<std::string>& args);
};