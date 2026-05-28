#include "store.hpp"

void KVStore::set(const std::string& key, const std::string& value){
    store.insert_or_assign(key, value);
}

std::optional<std::string> KVStore::get(const std::string& key)const {
    auto it = store.find(key);
    if (it != store.end()){
        return it->second;
    }
    return std::nullopt;
}

bool KVStore::remove(const std::string& key){
    return store.erase(key) > 0;
}

size_t KVStore::remove(const std::vector<std::string>& keys){
    size_t removed = 0;
    for (auto key : keys){
        removed += store.erase(key);
    }
    return removed;
}

bool KVStore::exists(const std::string& key)const{
    return store.find(key) != store.end();
}