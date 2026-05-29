#pragma once

#include <sstream>
#include <string>
#include <vector>

enum class Command {
    Set,
    Get,
    Del,
    Incr,
    IncrBy,
    Decr,
    DecrBy,
    Append,
    Quit,
    Unknown
};

namespace cli {
    std::string to_upper(std::string text);
    bool parse_set(std::istringstream& iss, std::string& key, std::string& value);
    bool parse_get(std::istringstream& iss, std::string& key);
    bool parse_del(std::istringstream& iss, std::vector<std::string>& keys);
    bool parse_incrby(std::istringstream& iss, std::string& key, int& num);
    bool parse_incr(std::istringstream& iss, std::string& key);
    bool parse_decr(std::istringstream& iss, std::string& key);
    bool parse_decrby(std::istringstream& iss, std::string& key, int& amount);
    bool parse_append(std::istringstream& iss, std::string& key, std::string& value);
    Command parse_command_type(std::string command);
}