#pragma once

#include <sstream>
#include <string>
#include <vector>

namespace cli {
    std::string to_upper(std::string text);
    bool parse_set(std::istringstream& iss, std::string& key, std::string& value);
    bool parse_get(std::istringstream& iss, std::string& key);
    bool parse_del(std::istringstream& iss, std::vector<std::string>& keys);
    bool parse_incrby(std::istringstream& iss, std::string& key, int& num);
}