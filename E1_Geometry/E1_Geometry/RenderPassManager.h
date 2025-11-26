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

	void RunPasses(ID3D11DeviceContext* deviceContext, ID3D11Device* device, const XMMATRIX& projectionMatrix) {
		for (auto pass : ActivePasses) {
			//Run passes set paramater function
			pass.second->setParamaterFunction(pass.second.Get());
			//Run pass render
			pass.second->pass->Render(deviceContext, device, projectionMatrix);
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

private:
	std::vector<std::pair<std::string, passInstance>> ActivePasses;
	InstanceCache<std::string, renderPassData> passCache;
};

/*
	Do i need to add more to this?

*/