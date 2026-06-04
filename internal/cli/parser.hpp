#pragma once

#include <sstream>
#include <string>
#include <vector>

namespace cli {

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

enum class ParseError {
    None,
    MissingKey,
    MissingValue,
    InvalidInteger,
    ExtraTokens,
    UnterminatedQuote
};

std::string error_message(ParseError error);
std::string to_upper(std::string text);

ParseError parse_value(std::istringstream& iss, std::string& key, std::string& value);
ParseError parse_get(std::istringstream& iss, std::string& key);
ParseError parse_del(std::istringstream& iss, std::vector<std::string>& keys);
ParseError parse_incrby(std::istringstream& iss, std::string& key, int& amount);
ParseError parse_incr(std::istringstream& iss, std::string& key);
ParseError parse_decr(std::istringstream& iss, std::string& key);
ParseError parse_decrby(std::istringstream& iss, std::string& key, int& amount);
Command parse_command_type(std::string command);

}