#pragma once
#ifndef _IDMAP_H_
#define _IDMAP_H_

#include <vector>
#include <unordered_map>
#include <memory>
#include <string>

template<typename value> 
class IDMap {
public:
	IDMap() : currentID(0) {};

	inline std::pair<size_t, std::shared_ptr<value>> emplaceID(value val) {
		size_t ID;
		if (!freeIDs.empty()) {
			ID = freeIDs.back();
			freeIDs.pop_back();
		} else {
			ID = currentID++;
		}
		auto [it, inserted] = map.emplace(ID, std::make_shared<value>(std::move(val)));
		if (!inserted) {
			throw std::runtime_error("IDMap error: duplicate ID inserted");
		}
		return std::make_pair( ID, it->second );
	}

	inline std::shared_ptr<value> getID(size_t ID) {
		auto it = map.find(ID);
		if (it != map.end()) {
			return it->second;
		}
		return {};
	}

	inline void eraseID(size_t ID) {
		if (map.find(ID) != map.end()) {
			map.erase(ID);
			freeIDs.push_back(ID);
		}
		else {
			throw std::runtime_error("Error: ID: " + std::to_string(ID) +
				" cannot be deleted as it does not exist!");
		}
	}

	template<typename Func>
	inline void forEach(Func func) {
		for (auto& [id, val] : map) {
			func(id, val);
		}
	}

	inline size_t size() { return map.size(); }

	inline void clear() {
		map.clear();
		freeIDs.clear();
		currentID = 0;
	}
private:
	std::unordered_map<size_t, std::shared_ptr<value>> map;
	std::vector<size_t> freeIDs;
	size_t currentID;
};

#endif