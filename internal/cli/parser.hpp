#pragma once

#include <sstream>
#include <string>

namespace cli {
    bool parse_set(std::istringstream& iss, std::string& key, std::string& value);
    bool parse_get(std::istringstream& iss, std::string& key);
}