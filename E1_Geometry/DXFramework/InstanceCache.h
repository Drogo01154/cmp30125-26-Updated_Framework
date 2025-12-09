#pragma once

#ifndef _INSTANCECACHE_H_
#define _INSTANCECACHE_H_

#include "LRUCache.h"
#include <type_traits>
#include <unordered_map>
#include <list>
#include <memory>
#include <string>
#include <functional>
#include <stdexcept>
#include "Converters.h"



template<typename IDType, typename ValueType>
class InstanceCache;

//Instance class used for instance owningCache
template<typename IDType, typename ValueType>
class Instance {
public:
	template<typename IDType, typename ValueType>
	friend class InstanceCache;

	//Copy constructor
	Instance(const Instance& other) noexcept
	{
		if (auto sp = other.value.lock()) {
			value = sp;
			ID = other.ID;
			ownedCache = other.ownedCache;
		}
		else {
			value = {};
			ID = {};
			ownedCache = nullptr;
		}

		checkIncrement();
	}

	//Move Constructor
	Instance(Instance&& other) noexcept {
		if (auto sp = other.value.lock()) {
			value = sp;
			ID = other.ID;
			ownedCache = other.ownedCache;

			other.value.reset();
			other.ID = {};
			other.ownedCache = nullptr;
		}
		else {
			value = {};
			ID = {};
			ownedCache = nullptr;
		}

	}

	//Equal operator
	Instance& operator=(const Instance& other) {
		if (this != &other) {
			//If other has valid value
			if (auto sp = other.value.lock()) {
				//If current value is valid and not same as other
				if (IsValid() && sp != value.lock()) {
					checkDeincrement();
				}
				value = sp;
				ID = other.ID;
				ownedCache = other.ownedCache;
			}
			else {
				if (IsValid()) {
					checkDeincrement();
				}
				value.reset();
				ownedCache = nullptr;
			}
			checkIncrement();
		}
		return *this;
	}

	//Eqaul Move operator
	Instance& operator=(Instance&& other) noexcept {
		//If other has valid value
		if (auto sp = other.value.lock()) {
			//If current value is valid and not same as other
			if (IsValid() && sp != value.lock()) {
				checkDeincrement();
			}

			//Swap values
			value = sp;
			ID = other.ID;
			ownedCache = other.ownedCache;

			other.value.reset();
			other.ID = {};
			other.ownedCache = nullptr;
		}
		else {
			//If currently valid
			if (IsValid()) {
				checkDeincrement();
			}
			value = {};
			ID = {};
			ownedCache = nullptr;
		}
		return *this;
	}

	//Returns if valid instance
	bool IsValid() const {
		return ownedCache != nullptr && value.lock() != nullptr;
	}

	//De-reference overload
	ValueType& operator*() {
		auto sp = value.lock();
		if (!sp)
			throw std::runtime_error("Attempted to access inaccessible variable");
		return sp->second;
	}

	//Ptr return overload
	ValueType* operator->() {
		auto sp = value.lock();
		return sp ? &sp->second : nullptr;
	}

	//Get functon
	ValueType* Get() {
		auto sp = value.lock();
		return sp ? &sp->second : nullptr;
	}

	//get ptr overload
	operator ValueType* () {
		auto sp = value.lock();
		return sp ? &sp->second : nullptr;
	}

	//Constructor
	Instance() : value{}, ID{}, ownedCache{ nullptr } {}

	~Instance() {
		checkDeincrement();
	}


private:

	void checkDeincrement() {
		if (auto sp = this->value.lock()) {
			sp->first--;
			if (sp->first <= 0) {
				ownedCache->RemoveID(ID, false);
			}
		}
	}

	void checkIncrement() {
		if (auto sp = this->value.lock()) {
			if (sp->first <= 0) {
				sp->first = 1;
				if (ownedCache->valueInitialiser && *ownedCache->valueInitialiser) {
					// Suppose you have a Value* pointer called val
					(*ownedCache->valueInitialiser)(&sp->second);
				}
				return;
			}
			sp->first++;
		}

	}

	Instance(InstanceCache<IDType, ValueType>* ownedCache, IDType ID, std::shared_ptr<std::pair<size_t, ValueType>> value) :
		ID(ID),
		ownedCache(ownedCache) {
		this->value = value;
		checkIncrement();
	}
	InstanceCache<IDType, ValueType>* ownedCache;
	IDType ID;	// Key for cache - numeric or string type
	std::weak_ptr<std::pair<size_t, ValueType>> value;
};

template<typename IDType, typename ValueType>
class InstanceCache {
public:
	using  InstanceData = std::pair<size_t, ValueType>;

	friend class Instance<IDType, ValueType>;

	//Constructor
	InstanceCache(uint64_t LRUSize = 10, bool RemoveOnZeroInstances = true) :
		entryCache(LRUSize),
		charVectorDirty(false),
		RemoveOnZeroInstances(RemoveOnZeroInstances),
		valueHandler(nullptr),
		valueInitialiser(nullptr)
	{
		//Assert that can only be numeric or string type
		static_assert(
			std::is_integral<IDType>::value ||
			std::is_same<IDType, std::string>::value ||
			std::is_same<IDType, std::wstring>::value,
			"InstanceCache can only be instantiated with integral or string ID types"
			);
		if constexpr (std::is_integral<IDType>::value) {	// Only compile this block if IDType is numeric
			//Set current ID index to start
			currentID = 0;
		}


	}
	/*
		Const ID emplace function - only works on string types
		Tries string as key
		bool determines if should return instance and instantiate value (run its construction function)
	*/
	inline Instance<IDType, ValueType> emplaceID(const IDType& ID, ValueType val, bool InstantiateInstance = true)
	{
		//Make sure only run on string types
		static_assert(!std::is_integral<IDType>::value, "Only string types allowed");
		//If already in map throw exception
		if (entries.find(ID) != entries.end()) {
			if constexpr (std::is_same<IDType, std::wstring>::value) { // If wstring
				throw std::runtime_error("ID already exists: " + Converters::convert_from_wstring(ID));
			}
			else {
				throw std::runtime_error("ID already exists: " + ID);
			}
		}

		auto dataPtr = std::make_shared<InstanceData>(0, std::move(val));
		entries[ID] = dataPtr;
		entryCache.EmplaceReplace(ID, dataPtr);
		charVectorDirty = true;
		if (InstantiateInstance) {	// return valued instance
			return Instance<IDType, ValueType>(this, ID, dataPtr);
		}
		else { // return empty instance

			return Instance<IDType, ValueType>();
		}
	}

	/*
		emplace function
		If numeric type determines ID and uses as key, if string type tries string as key
		bool determines if should return instance and instantiate value (run its construction function)
	*/
	inline Instance<IDType, ValueType> emplaceID(IDType& ID, ValueType val, bool InstantiateInstance = true) {
		if constexpr (std::is_integral<IDType>::value) {	// If numeric type value
			//Deterine ID value
			if (!freeIDs.empty()) {
				ID = freeIDs.back();
				freeIDs.pop_back();
			}
			else {
				ID = currentID++;
			}
		}
		else {
			if (ID.empty()) {
				throw std::runtime_error("String ID must be provided");
			}
			else if (entries.find(ID) != entries.end()) {
				return Instance<IDType, ValueType>();
			}
		}
		auto dataPtr = std::make_shared<InstanceData>(0, std::move(val));
		entries[ID] = dataPtr;
		entryCache.EmplaceReplace(ID, dataPtr);
		if (InstantiateInstance) {
			if (valueInitialiser && *valueInitialiser) {
				// Suppose you have a Value* pointer called val
				(*valueInitialiser)(&dataPtr->second);
			}
			return Instance<IDType, ValueType>(this, ID, dataPtr);
		}
		return Instance<IDType, ValueType>();
	}

	inline bool hasID(IDType ID) {
		return entryCache.contains(ID) || entries.find(ID) != entries.end();
	}

	inline size_t size() { return entries.size(); }

	inline void clear() {
		for (auto& it : entries) {
			if (valueHandler != nullptr) {
				(*valueHandler)(&it.second->second);
			}
		}
		entryCache.clear();
		entries.clear();
		if constexpr (std::is_integral_v<IDType>) {
			currentID = 0;
			freeIDs.clear();
		}
		charVectorDirty = true;
	}

	inline Instance<IDType, ValueType> getInstance(IDType ID) {
		std::shared_ptr<InstanceData> inst = findValue(ID, true);
		return Instance<IDType, ValueType> (this, ID, inst);
	}

	inline Instance<IDType, ValueType> tryGetInstance(IDType ID) {
		std::shared_ptr<InstanceData> inst = findValue(ID, false);
		if (inst) {
			Instance<IDType, ValueType> result(this, ID, inst);
			return result;
		}
		else {
			return Instance<IDType, ValueType>{};  // safely calls default ctor
		}

		return Instance<IDType, ValueType>{};  // safely calls default ctor
	}

	inline ValueType* tryGetValue(IDType ID) const {
		std::shared_ptr<InstanceData> instData = findValue(ID, false);
		if (instData) {
			return &instData->second;
		}
		return nullptr;
	}

	inline ValueType& getValue(IDType ID) const {
		return findValue(ID, true)->second;
	}

	inline size_t getIDRefCount(const IDType& ID) {
		auto LRUInstanceData = entryCache.get(ID);
		if (LRUInstanceData) return LRUInstanceData->first;
		else {
			auto it = entries.find(ID);
			if (it != entries.end()) {
				entryCache.EmplaceReplace(ID, it->second);
				return it->second->first;
			}
		}
		return 0;
	}

	inline bool RemoveID(const IDType& ID, bool forceDelete = true) {

		auto it = entries.find(ID);
		if (it != entries.end()) {
			if (valueHandler != nullptr) {
				(*valueHandler)(&it->second->second);
			}
			if (forceDelete || RemoveOnZeroInstances)
			{
				entryCache.Remove(ID);
				entries.erase(it);
				if constexpr (std::is_integral<IDType>::value) {
					freeIDs.push_back(ID);
				}
				charVectorDirty = true;
				return true;
			}
		}
		return false;
	}

	inline bool hasInstances(IDType ID) {
		if (auto LRUvalue = entryCache.get(ID)) {
			return LRUvalue->first > 0;
		}
		if (auto it = entries.find(ID); it != entries.end()) {
			return it->second->first > 0;
		}
		// No entry found — throw
		if constexpr (std::is_integral_v<IDType>) {
			throw std::runtime_error("Error: ID: " + std::to_string(ID) + " cannot be accessed!");
		}
		else if constexpr (std::is_same_v<IDType, std::wstring>) {
			throw std::runtime_error("Error: ID: " + Converters::convert_from_wstring(ID) + " cannot be accessed!");
		}
		else {
			throw std::runtime_error("Error: ID: " + ID + " cannot be accessed!");
		}
	}

	//Add function which runs when last instance of entry deleted
	template<typename Func>
	inline void addTypeEndHandler(Func func) {
		valueHandler = std::make_unique < std::function<void(ValueType*) >>(func);
	}


	//Add function which runs on first instance creation
	template<typename Func>
	inline void addTypeInitialiser(Func func) {
		valueInitialiser = std::make_unique<std::function<void(ValueType*)>>(func);
	}

	template<typename Func>
	inline void forEach(Func&& func) {
		for (auto& [id, val] : entries) {
			std::forward<Func>(func)(id, &val->second); // Light* can be modified
		}
	}

	inline const std::vector<std::string>& getCachedStringVec(bool forceUpdate = false) {
		updateCharVector(forceUpdate);
		return stringStorage;
	}


	inline const std::vector<const char*>& getCharVec(bool forceUpdate = false) {
		updateCharVector(forceUpdate);
		return charVec;
	}

	inline const std::string& getCachedName(size_t VecIndex, bool forceUpdate = false) {
		updateCharVector(forceUpdate);
		if (VecIndex > stringStorage.size()) { return ""; }
		return stringStorage[VecIndex];
	}

	inline bool changeID(const IDType& oldID, const IDType& newID) {
		static_assert(!std::is_integral<IDType>::value, "ChangeID is only allowed for string IDs");

		auto it = entries.find(oldID);
		if (it == entries.end()) { return false; }						// Old ID not found
		if (entries.find(newID) != entries.end()) { return false; }		// New ID found

		entries[newID] = it->second;
		entries.erase(it);
		entryCache.RenameKey(oldID, newID);
		charVectorDirty = true;
		return true;
	}

	void setDeleteNoInstances(bool value) {
		RemoveOnZeroInstances = value;
		if (RemoveOnZeroInstances) {
			for (auto it = entries.begin(); it != entries.end(); ) {
				if (it->second->first <= 0) {
					if constexpr (std::is_integral<IDType>::value) {
						freeIDs.push_back(it->first);
					}
					entryCache.Remove(it->first);
					entries.erase(it++);

					charVectorDirty = true;
				}
				else {
					++it;
				}
			}
		}
	}

private:



	inline std::shared_ptr<InstanceData> findValue(IDType ID, bool throwIfMissing = false) const {
		auto LRUInstanceData = entryCache.get(ID);
		std::shared_ptr<InstanceData> val = nullptr;
		if (LRUInstanceData) val = LRUInstanceData;
		else {
			auto it = entries.find(ID);
			if (it != entries.end()) {
				entryCache.EmplaceReplace(ID, it->second);
				val = it->second;
			}
		}

		if (!val && throwIfMissing) {
			if constexpr (std::is_integral<IDType>::value)
				throw std::runtime_error("Error: ID: " + std::to_string(ID) + " cannot be accessed!");
			else if constexpr (std::is_same<IDType, std::wstring>::value)
				throw std::runtime_error("Error: ID: " + Converters::convert_from_wstring(ID) + " cannot be accessed!");
			else
				throw std::runtime_error("Error: ID: " + ID + " cannot be accessed!");
		}

		return val;
	}

	void updateCharVector(bool forceUpdate = false) {
		if (!forceUpdate && !charVectorDirty) {
			return;
		}

		stringStorage.clear();
		charVec.clear();
		for (auto& [id, val] : entries) {
			if constexpr (std::is_integral<IDType>::value) {
				stringStorage.push_back(std::to_string(id));
			}
			else if constexpr (std::is_same<IDType, std::wstring>::value) {
				stringStorage.push_back(Converters::convert_from_wstring(id));
			}
			else {
				stringStorage.push_back(id);
			}
		}
		for (const std::string& it : stringStorage) {
			charVec.push_back(it.c_str());
		}
		charVectorDirty = false;
	}
	mutable LRUCache<IDType, std::shared_ptr<InstanceData>> entryCache;
	std::unordered_map<IDType, std::shared_ptr<InstanceData>> entries;
	std::vector<std::string> stringStorage;
	std::vector<const char*> charVec;
	std::vector<IDType> freeIDs;
	IDType currentID{};

	bool RemoveOnZeroInstances;
	bool charVectorDirty;

	std::unique_ptr<std::function<void(ValueType*)>> valueHandler;
	std::unique_ptr<std::function<void(ValueType*)>> valueInitialiser;
};

#endif