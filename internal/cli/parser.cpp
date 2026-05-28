#include "parser.hpp"

#include <iostream>
#include <sstream>

namespace cli {
    bool parse_set(std::istringstream &iss, std::string &key, std::string &value) {
        if (!(iss >> key)){
            std::cerr << "Error: SET requires a key and a value\n";
            return false;
        }
        iss >> std::ws;

        if (iss.peek() == '"'){
            iss.get();

            std::string rest;
            std::getline(iss, rest);

            std::size_t closing = rest.find('"');

            if (closing == std::string::npos){
                std::cerr << "Error: missing closing quote\n";
                return false;
            }
            value = rest.substr(0, closing);

            std::string after = rest.substr(closing +1);
            std::istringstream leftover(after);

            std::string extra;
            if (leftover >> extra){
                std::cerr << "Error: syntax error\n";
                return false;
            }
            return true;
        }

        if (!(iss >> value)){
            std::cerr << "Error: SET requires a value\n";
            return false;
        }

        std::string next;
        if (iss >> next){
            std::cerr << "Error: syntax error\n";
            return false;
        }
        return true;
    }

    

}