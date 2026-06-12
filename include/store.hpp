#pragma once

#include <shared_mutex>
#include <unordered_map>
#include <string>
#include <optional>
#include <vector>

// Thread-safe in-memory key-value store.
// Supports multiple concurrent readers and exclusive writers.
class KVStore {
    public:
    void set(const std::string& key, const std::string& value);
    std::optional<std::string> get(const std::string& key)const;
    bool exists(const std::string& key)const;
    std::size_t erase(const std::vector< std::string>& keys);
    std::optional<int>increase_by(const std::string& key, int num);
    std::string append(const std::string& key, const std::string& value);
    private:
    std::unordered_map<std::string, std::string>store;
    mutable std::shared_mutex mutex;

};