#include <algorithm>
#include <iostream>
#include <sstream>
#include <cctype>

#include "parser.hpp"

namespace cli {

    std::string error_message(ParseError error) {
        switch (error) {
            case ParseError::MissingKey:
                return "Error: missing key";

            case ParseError::MissingValue:
                return "Error: missing value";

            case ParseError::InvalidInteger:
                return "Error: invalid integer";

            case ParseError::ExtraTokens:
                return "Error: syntax error";

            case ParseError::UnterminatedQuote:
                return "Error: unterminated quote";

            default:
                return "";
            }
    }
    namespace  {
        std::string to_upper(std::string text) {
            std::transform(text.begin(), text.end(), text.begin(), [](unsigned char character) {
                return static_cast<char>(std::toupper(character));
            });

            return text;
        }

        bool extra_token(std::istringstream& iss) {
            iss >> std::ws;
            return !iss.eof();
        }
    
        ParseError parse_key_and_value(std::istringstream &iss, std::string &key, std::string &value) {
            if (!(iss >> key)){
                return ParseError::MissingKey;
            }

            iss >> std::ws;

            // Quoted values may contain spaces:
            // SET name "Bob Smith"
            if (iss.peek() == '"'){
                iss.get();

                std::string remaining_input;
                std::getline(iss, remaining_input);

                std::size_t closing_quote_pos = remaining_input.find('"');

                if (closing_quote_pos == std::string::npos){
                    return ParseError::UnterminatedQuote;
                }
                value = remaining_input.substr(0, closing_quote_pos);
                
                std::string trailing = remaining_input.substr(closing_quote_pos +1);
                std::istringstream trailing_stream(trailing);

                // Reject additional tokens after a quoted value.
                if (extra_token(trailing_stream)) {
                    return ParseError::ExtraTokens;
                }

                return ParseError::None;
            }

            if (!(iss >> value)){
                return ParseError::MissingValue;
            }

            if (extra_token(iss)){
                return ParseError::ExtraTokens;
            }

            return ParseError::None;
    }

    ParseError parse_single_key(std::istringstream& iss, std::string& key){
        if (!(iss >> key)) {
            return ParseError::MissingKey;
        }
        if (extra_token(iss)){
            return ParseError::ExtraTokens;
        }
        
        return ParseError::None;
    }

    ParseError parse_key_and_int(std::istringstream& iss, std::string& key, int& amount){
        if (!(iss >> key)){
            return ParseError::MissingKey;
        }

        if (!(iss >> amount)){
            if (iss.eof()){
                return ParseError::MissingValue;
            }
            return ParseError::InvalidInteger;
        }
        if (extra_token(iss)){
            return ParseError::ExtraTokens;
        }

        return ParseError::None;
        }    
    }
    

    Command parse_command_type(std::string command) {
        command = to_upper(command);

        if (command == "SET") return Command::Set;
        if (command == "GET") return Command::Get;
        if (command == "DEL") return Command::Del;
        if (command == "INCR") return Command::Incr;
        if (command == "INCRBY") return Command::IncrBy;
        if (command == "DECR") return Command::Decr;
        if (command == "DECRBY") return Command::DecrBy;
        if (command == "APPEND") return Command::Append;
        if (command == "QUIT" || command == "EXIT") return Command::Quit;

        return Command::Unknown;
    }

    ParseError parse_del(std::istringstream& iss, std::vector<std::string>& keys){
        // DEL accepts one or more keys:
        // DEL key1 key2 key3
        std::string key;

        while (iss >> key){
            keys.push_back(key);
        }
        if (keys.empty()){
            return ParseError::MissingKey;
        }
        return ParseError::None;

    }

    ParseError parse_set(std::istringstream &iss, std::string &key, std::string &value){
        return parse_key_and_value(iss, key, value);
    }

    ParseError parse_append(std::istringstream &iss, std::string &key, std::string &value){
        return parse_key_and_value(iss, key, value);
    }

    ParseError parse_get(std::istringstream &iss, std::string &key){
        return parse_single_key(iss, key);
    }

    ParseError parse_incr(std::istringstream& iss, std::string& key) {
        return parse_single_key(iss, key);
    }

    ParseError parse_decr(std::istringstream &iss, std::string &key){
        return parse_single_key(iss, key);
    }

    ParseError parse_incrby(std::istringstream& iss, std::string& key, int& amount) {
        return parse_key_and_int(iss, key, amount);
    }

    ParseError parse_decrby(std::istringstream& iss, std::string& key, int& amount) {
        return parse_key_and_int(iss, key, amount);
    }

}