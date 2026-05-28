#include "store.hpp"

void KVStore::set(const std::string& key,
                  const std::string& value) {

    store.insert_or_assign(key, value);
}

std::optional<std::string>
KVStore::get(const std::string& key) const {

    if (auto it = store.find(key);
        it != store.end()) {

        return it->second;
    }

    return std::nullopt;
}

bool KVStore::remove(const std::string& key) {
    return store.erase(key) > 0;
}

std::size_t
KVStore::remove(const std::vector<std::string>& keys) {

    std::size_t removed = 0;

    for (const auto& key : keys) {
        removed += store.erase(key);
    }

    return removed;
}

bool KVStore::exists(const std::string& key) const {
    return store.find(key) != store.end();
}