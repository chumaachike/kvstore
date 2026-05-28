#pragma once
#include <unordered_map>
#include <string>
#include <optional>
#include <vector>


class KVStore {
    public:
    void set(const std::string& key, const std::string& value);
    std::optional<std::string> get(const std::string& key)const;
    bool remove(const std::string& key);
    bool exists(const std::string& key)const;
    std::size_t remove(const std::vector< std::string>& keys);
    private:
    std::unordered_map<std::string, std::string>store;

};