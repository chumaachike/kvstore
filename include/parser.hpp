// #pragma once


// #include <string>
// #include <vector>

// namespace cli {






// std::string error_message(ParseError error);

// // Converts command text to a Command enum.
// // The match is case-insensitive.
// Command parse_command_type(std::string command);

// // Parses: SET <key> <value>
// // Quoted values may contain spaces.
// ParseError parse_set(std::istringstream& iss, std::string& key, std::string& value);

// // Parses: APPEND <key> <value>
// // Quoted values may contain spaces.
// ParseError parse_append(std::istringstream& iss, std::string& key, std::string& value);

// // Parses: GET <key>
// ParseError parse_get(std::istringstream& iss, std::string& key);

// // Parses: DEL <key> [key...]
// ParseError parse_del(std::istringstream& iss, std::vector<std::string>& keys);

// // Parses: INCR <key>
// ParseError parse_incr(std::istringstream& iss, std::string& key);

// // Parses: INCRBY <key> <amount>
// ParseError parse_incrby(std::istringstream& iss, std::string& key, int& amount);

// // Parses: DECR <key>
// ParseError parse_decr(std::istringstream& iss, std::string& key);

// // Parses: DECRBY <key> <amount>
// ParseError parse_decrby(std::istringstream& iss, std::string& key, int& amount);

// }

// parser.hpp
#pragma once

#include <string>
#include <vector>
#include <expected>

enum class ParseError  {
    EmptyInput,
    InvalidArguments,
    InvalidInteger,
    UnterminatedQuote,
    UnknownCommand
};

enum class CommandType {
    Set,
    Get,
    Del,
    Incr,
    IncrBy,
    Decr,
    DecrBy,
    Append,
    Quit,
    Invalid
};

struct Command {
    CommandType type;
    std::vector<std::string> args;
};

template<typename T, typename E>
using ParseResult = std::expected<T, E>;

class Parser {
public:
    ParseResult<Command, ParseError> parse(const std::string& input);

private:
    static std::vector<std::string> tokenize(const std::string& input);
    static std::string to_upper(std::string text);
    ParseResult<Command, ParseError>parse_key_value(const CommandType& command, const std::vector<std::string>& remaining_tokens);
    ParseResult<Command, ParseError>parse_single_key(const CommandType& command, const std::vector<std::string>& remaining_tokens);
    ParseResult<Command, ParseError>parse_key_and_int(const CommandType& command, const std::vector<std::string>& remaining_tokens);
};