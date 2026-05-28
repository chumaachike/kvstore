#include "parser.hpp"

#include <algorithm>
#include <iostream>
#include <sstream>

namespace cli {

    std::string to_upper(std::string text) {
    std::transform(
        text.begin(),
        text.end(),
        text.begin(),
        [](unsigned char character) {
            return std::toupper(character);
        }
    );

    return text;
}
    
    bool parse_set(std::istringstream &iss, std::string &key, std::string &value) {

        // SET requires at least:
        // SET <key> <value>
        if (!(iss >> key)){
            std::cerr << "Error: SET requires a key and a value\n";
            return false;
        }

        // Quoted values allow spaces:
        // SET name "Bob Smith"
        iss >> std::ws;

        if (iss.peek() == '"'){
            iss.get(); // consume opening quote

            std::string remaining_input;
            std::getline(iss, remaining_input);

            std::size_t closing_quote_pos = remaining_input.find('"');

            if (closing_quote_pos == std::string::npos){
                std::cerr << "Error: missing closing quote\n";
                return false;
            }
            value = remaining_input.substr(0, closing_quote_pos);
            
            // Reject extra tokens after quoted value
            std::string trailing = remaining_input.substr(closing_quote_pos +1);
            std::istringstream trailing_stream(trailing);

            std::string extra_token;
            if (trailing_stream >> extra_token){
                std::cerr << "Error: syntax error\n";
                return false;
            }

            return true;
        }

        // Non-quoted values cannot contain spaces
        if (!(iss >> value)){
            std::cerr << "Error: SET requires a value\n";
            return false;
        }

        // Reject extra tokens:
        // SET name Bob extra
        std::string extra_token;
        if (iss >> extra_token){
            std::cerr << "Error: syntax error\n";
            return false;
        }

        return true;
    }

    bool parse_get(std::istringstream &iss, std::string &key){
        // GET requires at least:
        // GET <key>
        if (!(iss >> key)){
            std::cerr << "Error GET requires a key\n";
            return false;
        }
        // Reject extra tokens:
        std::string next;
        if(iss >> next){
            std::cerr << "Error: syntax error \n";
            return false;
        }
        return true;
    }

    bool parse_del(std::istringstream& iss, std::vector<std::string>& keys){
        // DEL requires at least:
        // DEL <key>
        std::string key;

        while (iss >> key){
            keys.push_back(key);
        }
        if (keys.empty()){
            std::cerr << "Error: DEL requires at least one key\n";
            return false;
        }
        return true;

    }
    bool parse_incrby(std::istringstream& iss,
                  std::string& key,
                  int& amount) {
    // INCRBY requires:
    // INCRBY <key> <amount>
    if (!(iss >> key)) {
        std::cerr << "Error: INCRBY requires a key\n";
        return false;
    }

    if (!(iss >> amount)) {
        std::cerr << "Error: INCRBY requires an integer\n";
        return false;
    }

    std::string extra_token;
    if (iss >> extra_token) {
        std::cerr << "Error: syntax error\n";
        return false;
    }

    return true;
    }

    bool parse_incr(std::istringstream& iss,
                std::string& key) {

    // INCR requires:
    // INCR <key>
    if (!(iss >> key)) {
        std::cerr << "Error: INCR requires a key\n";
        return false;
    }

    std::string extra_token;
    if (iss >> extra_token) {
        std::cerr << "Error: syntax error\n";
        return false;
    }

    return true;
}

bool parse_decr(std::istringstream &iss, std::string &key){
    // DECR requires:
    // DECR <key>
    if (!(iss >> key)) {
        std::cerr << "Error: DECR requires a key\n";
        return false;
    }

    std::string extra_token;
    if (iss >> extra_token){
        std::cerr << "Error syntax error\n";
        return false;
    }
    return true;
    }
bool parse_decrby(std::istringstream& iss, std::string& key, int& amount) {
    // DECRBY requires:
    // DECRBY <key> <amount>
    if (!(iss >> key)) {
        std::cerr << "Error: DECRBY requires a key\n";
        return false;
    }

    if (!(iss >> amount)) {
        std::cerr << "Error: DECRBY requires an integer\n";
        return false;
    }

    std::string extra_token;
    if (iss >> extra_token) {
        std::cerr << "Error: syntax error\n";
        return false;
    }

    return true;
}

}