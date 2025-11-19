#pragma once

#pragma once

#ifndef _INSTANCEMAP_H_
#define _INSTANCEMAP_H_

#include "LRUCache.h"
#include <functional>

template<typename Value>
class InstanceCache;

//Instance class used for instance map
template<typename Value>
class Instance {
public:

	Instance() {
		map = nullptr;
		valuePtr = nullptr;
		ID = SIZE_MAX;
	}

	Instance(InstanceCache<Value>* map, size_t id, std::shared_ptr<Value> valuePtr)
	{
		map->IncrementID(ID);
	}

	Instance(const Instance& other) {
		map = other.map;
		ID = other.ID;
		valuePtr = other.valuePtr;
		map->IncrementID(ID);
	}

	Instance& operator=(const Instance& other) {
		map = other.map;
		ID = other.ID;
		valuePtr = other.valuePtr;
		map->IncrementID(ID);
		return *this;
	}

	~Instance() {
		map->DeincrementID(ID);
	}

	Value& operator*() {
		return *valuePtr;
	}

	Value* operator->() {
		if (!valuePtr) {
			throw std::runtime_error("Null pointer access via operator->");
		}
		return valuePtr.get();
	}

	operator std::shared_ptr<Value>() {
		if (!valuePtr) {
			throw std::runtime_error("Null pointer access via operator->");
		}
		return valuePtr;
	}
private:
	InstanceCache<Value>* map;
	size_t ID;
	std::shared_ptr<Value> valuePtr;
};

template<typename Value>
class InstanceCache {
public:
	friend class Instance<Value>;

	InstanceCache() : currentID(0), valueDestructor(nullptr) {}

	inline Instance<Value> emplaceID(size_t& ID, Value val) {
		if (!freeIDs.empty()) {
			ID = freeIDs.back();
			freeIDs.pop_back();
		}
		else {
			ID = currentID++;
		}
		auto it = entries.emplace(ID, std::make_pair(0, std::make_shared<Value>(std::move(val))));
		entryCache.EmplaceReplace(ID, &it.first->second);
		return Instance<Value>(this, ID, it.first->second.second);
	}

	inline Instance<Value> getID(size_t ID) {
		//Check LRU cache
		std::pair<size_t, std::shared_ptr<Value>>* LRUvalue = entryCache.get(ID);
		if (LRUvalue) {
			return Instance<Value>(this, ID, LRUvalue->second);
		}
		else {
			auto mapValue = entries.find(ID);
			if (mapValue != entries.end()) {
				entryCache.EmplaceReplace(ID, &mapValue->second);
				return Instance<Value>(this, ID, mapValue->second.second);
			}
		}
		throw std::runtime_error("Error: ID: " + std::to_string(ID) +
			" cannot be accessed as it does not exist!");
	}

	inline size_t size() { return entries.size(); }

	inline void clear() {
		entryCache.clear();
		entries.clear();
		freeIDs.clear();
		currentID = 0;
	}

	template<typename Func>
	inline void addTypeDestructor(Func func) {
		valueDestructor = std::make_unique<std::function<void(std::shared_ptr<Value>)>>(func);
	}

	template<typename Func>
	inline void forEach(Func func) {
		for (auto& [id, val] : entries) {
			func(id, val);
		}
	}

private:
	void IncrementID(size_t ID) {
		entries[ID].first++;
	}

	void DeincrementID(size_t ID) {
		auto it = entries.find(ID);
		if (it != entries.end()) {
			it->second.first--;
			if (it->second.first == 0) {
				if (valueDestructor != nullptr) {
					(*valueDestructor)(it->second.second);
				}
				entryCache.Remove(ID);
				entries.erase(it);
				freeIDs.push_back(ID);
			}
		}
	}

	LRUCache<size_t, std::pair<size_t, std::shared_ptr<Value>>*> entryCache;
	std::unordered_map<size_t, std::pair<size_t, std::shared_ptr<Value>>> entries;

	std::vector<size_t> freeIDs;
	size_t currentID;
	std::unique_ptr<std::function<void(std::shared_ptr<Value>)>> valueDestructor;
};

#endif