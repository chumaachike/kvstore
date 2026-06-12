#pragma once


#include <string>
#include <vector>

namespace cli {

// Supported CLI commands for the KV store.
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

// Possible validation errors returned by command parsers.
enum class ParseError {
    None,
    MissingKey,
    MissingValue,
    InvalidInteger,
    ExtraTokens,
    UnterminatedQuote
};

std::string error_message(ParseError error);

// Converts command text to a Command enum.
// The match is case-insensitive.
Command parse_command_type(std::string command);

// Parses: SET <key> <value>
// Quoted values may contain spaces.
ParseError parse_set(std::istringstream& iss, std::string& key, std::string& value);

// Parses: APPEND <key> <value>
// Quoted values may contain spaces.
ParseError parse_append(std::istringstream& iss, std::string& key, std::string& value);

// Parses: GET <key>
ParseError parse_get(std::istringstream& iss, std::string& key);

// Parses: DEL <key> [key...]
ParseError parse_del(std::istringstream& iss, std::vector<std::string>& keys);

// Parses: INCR <key>
ParseError parse_incr(std::istringstream& iss, std::string& key);

// Parses: INCRBY <key> <amount>
ParseError parse_incrby(std::istringstream& iss, std::string& key, int& amount);

// Parses: DECR <key>
ParseError parse_decr(std::istringstream& iss, std::string& key);

// Parses: DECRBY <key> <amount>
ParseError parse_decrby(std::istringstream& iss, std::string& key, int& amount);

}