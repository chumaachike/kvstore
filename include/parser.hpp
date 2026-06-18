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