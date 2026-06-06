#include "store.hpp"

void KVStore::set(const std::string& key, const std::string& value) {
    std::lock_guard  lock(mutex);
    store.insert_or_assign(key, value);
}

std::optional<std::string> KVStore::get(const std::string& key) const {
    std::shared_lock lock(mutex);

    if (auto it = store.find(key);
        it != store.end()) {

        return it->second;
    }

    return std::nullopt;
}

std::size_t KVStore::remove(const std::vector<std::string>& keys) {
    std::lock_guard lock(mutex);

    std::size_t removed = 0;

    for (const auto& key : keys) {
        removed += store.erase(key);
    }

    return removed;
}

bool KVStore::exists(const std::string& key) const {
    std::shared_lock lock(mutex);
    return store.find(key) != store.end();
}

std::optional<int> KVStore::increase_by(const std::string& key, int amount) {
    std::lock_guard lock (mutex);
    auto it = store.find(key);

    if (it == store.end()) {
        store.emplace(key, std::to_string(amount));
        return amount;
    }

    try {
        std::size_t parsed_chars = 0;
        int current_value = std::stoi(it->second, &parsed_chars);

        if (parsed_chars != it->second.size()) {
            return std::nullopt;
        }

        int new_value = current_value + amount;

        it->second = std::to_string(new_value);
        return new_value;
    } catch (const std::exception&) {
        return std::nullopt;
    } 
}

std::string KVStore::append(const std::string& key, const std::string& value) {
    std::lock_guard lock (mutex);
    auto it = store.find(key);

    if (it == store.end()) {
        store.emplace(key, value);
        return value;
    }

    it->second.append(value);
    return it->second;
}