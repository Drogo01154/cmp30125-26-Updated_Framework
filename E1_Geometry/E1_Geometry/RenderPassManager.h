#pragma once
#include "RenderPass.h"
#include <memory>
/*
	Class for managing render passes
*/
#define passConstructionFunction std::function<std::shared_ptr<renderPass>(size_t, passDependancies&)>

struct PassData {
	PassData(std::vector<std::string> dependancies, passConstructionFunction func) :
		constructionFunction(std::move(func)), dependancies(std::move(dependancies)) {
	}
	passConstructionFunction constructionFunction;
	std::vector<std::string> dependancies;
};


class RenderPassManager {
public:
	void calcualtePassIndexes() {
		activePassIndexes.clear();
		for (size_t i = 0; i < activePassList.size(); ++i) {
			activePassIndexes.emplace(activePassList[i].first, i);
		}
	}

	void RenderAll(ID3D11DeviceContext* deviceContext, ID3D11Device* device) {
		for (auto& [name, pass] : activePassList) {
			pass->Render(deviceContext, device);
		}
	}

	std::shared_ptr<renderPass> InitPass(const std::string& name) {
		//If pass alaready active return it;
		if (activePassIndexes.contains(name)) { 
			return activePassList[activePassIndexes[name]].second; } // Break as already in pass list
		std::unordered_map<std::string, std::shared_ptr<renderPass>> dependencies;
		
		PassData& data = potentialPasses.at(name);
		for (auto pass : data.dependancies) {
			auto passIndex = activePassIndexes.find(pass);
			if (passIndex != activePassIndexes.end()) {
				dependencies.emplace(pass, activePassList[passIndex->second].second);
			} else {
				dependencies.emplace(pass, InitPass(pass));
			}
		}
		size_t index = activePassList.size();
		auto ptr = data.constructionFunction(index, dependencies);
		activePassList.push_back(std::make_pair(name, ptr));
		activePassIndexes[name] = index;
		return ptr;
	}

	template<typename PassType, typename... Args>
	void AddPassConstructionFunction(const std::string& name, std::vector<std::string> dependancies, Args&&... args) {
		auto argsTuple = std::make_tuple(std::forward<Args>(args)...);

		passConstructionFunction constructionFunction = [argsTuple = std::move(argsTuple)](size_t stage, passDependancies& deps) -> std::shared_ptr<renderPass> {
			// Capture argsTuple by value and use it to construct PassType
			return std::apply(
				[stage, &deps](auto&&... unpackedArgs) -> std::shared_ptr<renderPass> {
					return std::make_shared<PassType>(stage, deps, std::forward<decltype(unpackedArgs)>(unpackedArgs)...);
				},
				argsTuple
			);
			};

		// Now store it in your potentialPasses map with empty dependencies by default
		potentialPasses.emplace(name, PassData(dependancies, std::move(constructionFunction)));
	}
	
private:
	std::unordered_map<std::string, PassData> potentialPasses;
	std::vector<std::pair<std::string, std::shared_ptr<renderPass>>> activePassList;
	std::unordered_map<std::string, size_t> activePassIndexes;
};