#include "parser.hpp"

#include <sstream>
#include <algorithm>
#include <cctype>

ParseResult<Command, ParseError> Parser::parse(const std::string& input) {
    auto tokens = tokenize(input);

    if (tokens.empty()) {
        return std::unexpected(ParseError::EmptyInput);
    }

    std::string command = to_upper(tokens[0]);

    if (command == "SET") {
        return parse_key_value(CommandType::Set, {tokens.begin()+1, tokens.end()});
    }

    if (command == "GET") {
        return parse_single_key(CommandType::Get, {tokens.begin()+1, tokens.end()});
    }

    if (command == "DEL") {
        if (tokens.size() < 2) return std::unexpected(ParseError::InvalidArguments);

        std::vector<std::string> keys;
        for (std::size_t i = 1; i < tokens.size(); ++i) {
            keys.push_back(tokens[i]);
        }

        return Command{CommandType::Del, keys};
    }

    if (command == "INCR") {
        return parse_single_key(CommandType::Incr, {tokens.begin()+1, tokens.end()});
    }

    if (command == "INCRBY") {
        return parse_key_and_int(CommandType::IncrBy, {tokens.begin()+1, tokens.end()});
    }

    if (command == "DECR") {
        return parse_single_key(CommandType::Decr, {tokens.begin()+1, tokens.end()});
    }

    if (command == "DECRBY") {
        
        return parse_key_and_int(CommandType::DecrBy, {tokens.begin()+1, tokens.end()});
    }

    if (command == "APPEND") {
        return parse_key_value(CommandType::Append, {tokens.begin()+1, tokens.end()});
    }

    if (command == "QUIT") {
        if (tokens.size() != 1) return std::unexpected(ParseError::InvalidArguments);
        return Command{CommandType::Quit, {}};
    }

    return std::unexpected(ParseError::UnknownCommand);
}

std::vector<std::string> Parser::tokenize(const std::string& input) {
    std::istringstream iss(input);

    std::vector<std::string> tokens;
    std::string token;

    while (iss >> token) {
        tokens.push_back(token);
    }

    return tokens;
}

std::string Parser::to_upper(std::string text) {
    std::transform(
        text.begin(),
        text.end(),
        text.begin(),
        [](unsigned char c) {
            return std::toupper(c);
        }
    );

    return text;
}

ParseResult<Command, ParseError> Parser::parse_key_value(const CommandType& command, const std::vector<std::string>& remaining_tokens) {
    if (remaining_tokens.size() < 2) {
        return std::unexpected(ParseError::InvalidArguments);
    }

    std::string key = remaining_tokens[0];
    std::string value;

    const std::string& first_value_token = remaining_tokens[1];

    // Quoted value
    if (!first_value_token.empty() && first_value_token.front() == '"') {
        bool closed = false;
        std::size_t closing_index = 0;

        for (std::size_t i = 1; i < remaining_tokens.size(); ++i) {
            if (i > 1) {
                value += ' ';
            }

            value += remaining_tokens[i];

            // Check different variations of closing quote 
            bool is_closing_quote =
                value.size() >= 2 &&
                value.back() == '"';

            if (is_closing_quote) {
                closed = true;
                closing_index = i;
                break;
            }
        }

        if (!closed) {
            return std::unexpected(ParseError::UnterminatedQuote);
        }

        // Anything after closing quote is invalid: SET name "Edward" extra
        if (closing_index != remaining_tokens.size() - 1) {
            return std::unexpected(ParseError::InvalidArguments);
        }

        value.erase(value.begin()); // remove opening quote
        value.pop_back();          // remove closing quote

        return Command{command, {key, value}};
    }

    // Unquoted value
    if (remaining_tokens.size() != 2) {
        return std::unexpected(ParseError::InvalidArguments);
    }

    value = remaining_tokens[1];

    return Command{command, {key, value}};
}

ParseResult<Command, ParseError>Parser::parse_single_key(const CommandType& command, const std::vector<std::string>& remaining_tokens){
     if (remaining_tokens.size() != 1) return std::unexpected(ParseError::InvalidArguments);
        return Command{command, {remaining_tokens[0]}};
}

ParseResult<Command, ParseError> Parser::parse_key_and_int(
    const CommandType& command,
    const std::vector<std::string>& remaining_tokens
) {
    if (remaining_tokens.size() != 2) {
        return std::unexpected(ParseError::InvalidArguments);
    }

    try {
        std::size_t pos = 0;
        std::stoi(remaining_tokens[1], &pos);

        if (pos != remaining_tokens[1].size()) {
            return std::unexpected(ParseError::InvalidInteger);
        }
    } catch (...) {
        return std::unexpected(ParseError::InvalidArguments);
    }

    return Command{command, remaining_tokens};
}

std::string_view Parser::parse_error(ParseError error){
    switch (error) {
        case ParseError::EmptyInput:
            return "";

        case ParseError::InvalidArguments:
            return "ERR invalid arguments";

        case ParseError::InvalidInteger:
            return "ERR invalid integer";

        case ParseError::UnterminatedQuote:
            return "ERR unterminated quote";

        case ParseError::UnknownCommand:
            return "ERR unknown command";
    }

    return "ERR unknown parse error";
}