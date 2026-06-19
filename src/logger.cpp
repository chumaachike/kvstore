#include <stdexcept>

#include "logger.hpp"

AOFLogger::AOFLogger(const std::string& path)
    : out(path, std::ios::app) {
        if (!out) throw std::runtime_error("Could not open AOF file");
    }

AOFLogger::~AOFLogger() {
    out.close();
}

void AOFLogger::append(const std::string& line){
    std::lock_guard lock(mutex);
    out << line << '\n';
    out.flush();
    
}