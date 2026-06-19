#pragma once
#include <string>
#include<fstream>
#include <mutex>


class AOFLogger {
public:
    explicit AOFLogger(const std::string& path);

    void append(const std::string& command);
    void close();
    void reopen(const std::string& path);

    ~AOFLogger();
private:
    void open(const std::string& path);
    std::ofstream out;
    std::mutex mutex;
};