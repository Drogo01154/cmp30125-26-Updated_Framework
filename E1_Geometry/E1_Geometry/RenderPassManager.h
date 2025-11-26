#pragma once
#include "RenderPass.h"
#include <memory>
/*
	Class for managing render passes
*/

#define passInstance Instance<std::string, renderPassData>

struct renderPassData {
	renderPassData(const std::string& name, std::function<std::shared_ptr<renderPass>()> constructPass) :
	name(name), constructPass(constructPass) {}
	std::string name;
	std::shared_ptr<renderPass> pass;
	std::function<std::shared_ptr<renderPass>()> constructPass;
	std::function<void(renderPassData*)> setParamaterFunction;
};

class RenderPassManager
{
public:
	RenderPassManager() {
		passCache.setDeleteNoInstances(false);
		passCache.addTypeInitialiser([](renderPassData* data) {
			data->pass = data->constructPass();
		});
		passCache.addTypeEndHandler([](renderPassData* data) {
			data->pass.reset();
		});
	};

	void RunPasses(ID3D11Device* device) {
		for (auto pass : ActivePasses) {
			//Run passes set paramater function
			pass.second->setParamaterFunction(pass.second.Get());
			//Run pass render
			pass.second->pass->Render(device);
		}
	}

	void ImGuiMenus() {
		passCache.forEach([](const std::string& ID, renderPassData* passData) {
			if (passData->pass) {
				passData->pass->ImGuiMenu();
			}
		});
	}
	
	template<typename PassType, typename... Args>
	void addPassToCache(const std::string& name, Args&&... args) {
		auto argsTuple = std::make_tuple();

		passCache.emplaceID(name, 
				name, 
				[argsTuple]() -> std::shared_ptr<renderPass> {
					return std::apply([](auto&&... unpackedArgs) {
						return std::make_shared<renderPass>(std::forward<decltype(unpackedArgs)>(unpackedArgs)...);
					}, argsTuple);
				}
			)
		);
	}

	template<typename PassType, typename... Args>
	void addPassParamaterFunction(const std::string& name, Args&&... args) {
		auto argsTuple = std::make_tuple(std::forward<Args>(args)...);
		renderPassData& data = passCache.getValue(name);
		data.setParamaterFunction = [argsTuple](renderPassData* data) {
			std::apply([](auto&&... unpackedArgs) {
				std::shared_ptr<PassType> typeCast = std::dynamic_pointer_cast<PassType>(data->pass);
				typeCast->setParameters(std::forward<decltype(unpackedArgs)>(unpackedArgs)...)
				}, argsTuple);
			}
		
	}

	/*		renderPassData test(
			name,
			[argsTuple]() -> std::shared_ptr<renderPass> {
				
			}
		);
		
		passCache.emplaceID(
			name,
			
		);
	}

	template<typename PassType, typename ParamaterFunction, typename... Args>
	void addPassSetParamaterFunction(const std::string& name, Args&&... args) {

	}
	*/
	/*void addPassToCache(std::string name, )

	void addPass(std::unique_ptr<renderPass> pass) {
		renderPasses.push_back(std::move(pass));
	}

	void renderAll() {
		for (auto& pass : renderPasses) {
			pass->Render();
		}
	}

	void removePass(size_t  ID) {
		if (ID < renderPasses.size()) {
			renderPasses.erase(renderPasses.begin() + ID);
		}
		else {
			throw std::runtime_error("Error: Pass " + std::to_string(ID) + " does not exist!");
		}
	}

	renderPass* getPass(size_t ID) {
		if (ID < renderPasses.size()) {
			return renderPasses[ID].get();
		}
		return nullptr;
	}

	void clearPasses() { renderPasses.clear(); }
	*/
private:
	std::vector<std::pair<std::string, passInstance>> ActivePasses;
	InstanceCache<std::string, renderPassData> passCache;
};

/*
	Do i need to add more to this?

*/