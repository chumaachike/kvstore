#pragma once
#include <string>
#include<fstream>
#include <mutex>


class AOFLogger {
public:
    explicit AOFLogger(const std::string& path);
    void append(const std::string& command);
    ~AOFLogger();
private:
    std::ofstream out;
    std::mutex mutex;
};