#include <stdexcept>

#include "logger.hpp"

AOFLogger::AOFLogger(const std::string& path){
    open(path);
}

AOFLogger::~AOFLogger() {
    close();
}

void AOFLogger::append(const std::string& line){
    std::lock_guard lock(mutex);
    out << line << '\n';
    out.flush();
    
}

void AOFLogger::close(){
    std::lock_guard lock(mutex);
    out.close();
}

void AOFLogger::open(const std::string& path){
    std::lock_guard lock (mutex);
    out.open(path, std::ios::app);
    if (!out) throw std::runtime_error("Could not open AOF file"); 
}

void AOFLogger::reopen(const std::string& path){
    std::lock_guard lock (mutex);

    out.close();
    out.open(path, std::ios::app);

    if(!out) throw std::runtime_error("could not open AOF file");

}