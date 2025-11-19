#pragma once

#ifndef _LRUCACHE_H_
#define _LRUCACHE_H_

#include <unordered_map>
#include <list>
#include <memory>

//Custom struct template to return if value unique ptr
template<typename T>
struct is_unique_ptr : std::false_type {};

template<typename T, typename D>
struct is_unique_ptr<std::unique_ptr<T, D>> : std::true_type {};

template<typename Key, typename Value>
    requires std::regular<Key>&& requires(Key k) { std::hash<Key>{}(k); }
class LRUCache
{
    uint64_t capacity;
    std::unordered_map<Key, typename std::list<std::pair<Key, Value>>::iterator> cache;
    std::list<std::pair<Key, Value>> order;

public:

    //Constructor
    LRUCache() : capacity(10) {}
    LRUCache(uint64_t _capacity) : capacity(_capacity) {}

    inline void setCapacity(uint64_t _capacity) { capacity = _capacity; }

    //Get for if ponter type
    template <typename T = Value>
    Value get(const Key& key)
        requires(std::is_pointer_v<T> || requires { typename T::element_type; }) {
        auto it = cache.find(key);
        if (it == cache.end()) {
            return Value(); // nullptr, empty shared_ptr, etc.
        }

        order.splice(order.begin(), order, it->second);

        if constexpr (is_unique_ptr<Value>::value) {
            // Move only for unique_ptr
            return std::move(it->second->second);
        }
        else {
            // Copy for shared_ptr or raw pointer
            return it->second->second;
        }
    }

    template <typename T = Value>
    Value* get(const Key& key)
        requires(!std::is_pointer_v<T> && !requires { typename T::element_type; }) {
        auto it = cache.find(key);
        if (it == cache.end()) {
            return nullptr;
        }

        order.splice(order.begin(), order, it->second);
        return &it->second->second;
    }

    template<typename K, typename V>
    inline void EmplaceReplace(K&& _Key, V&& _Value) {
        auto it = cache.find(_Key); // Check cache for key
        if (it != cache.end()) { // if key already in cache replace it
            order.splice(order.begin(), order, it->second);
            it->second->second = std::forward<V>(_Value);
        }
        else {
            if (order.size() == capacity) {
                Key lruKey = std::move(order.back().first);
                order.pop_back(); // remove least recently used
                cache.erase(lruKey);
            }

            order.emplace_front(std::forward<K>(_Key), std::forward<V>(_Value));
            cache[order.front().first] = order.begin();
        }
    }

    inline void Remove(const Key& key) {
        auto it = cache.find(key);
        if (it != cache.end()) {
            order.erase(it->second);
            cache.erase(it);
        }
    }

    inline void clear() {
        cache.clear();
        order.clear();
    }
};

#endif
