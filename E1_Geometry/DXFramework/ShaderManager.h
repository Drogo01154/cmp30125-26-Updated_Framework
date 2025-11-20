#pragma once

#ifndef _SHADERMANAGER_H_
#define _SHADERMANAGER_H_

#include "BaseShader.h"
#include "BaseShaderModule.h"
#include "InstanceCache.h"
#include "Material.h"
#include <functional>

struct ShaderData {
	ShaderData(const std::string& name, std::function<std::shared_ptr<BaseShader>()> constructShader) :
	name(name), constructShader(constructShader) {}
	std::string name;

	// Factory function that creates the shader
	std::function<std::shared_ptr<BaseShader>()> constructShader;
	std::shared_ptr<BaseShader> shader;
};

struct ModuleData {
	ModuleData(const std::string& name, std::function<std::shared_ptr<BaseShaderModule>()> constructModule) :
		name(name), constructModule(constructModule) {
	}
	std::string name;
	// Factory function that creates the shader
	std::function<std::shared_ptr<BaseShaderModule>()> constructModule;
	std::shared_ptr<BaseShaderModule> module;
};

#define ShaderInstance Instance<std::string, ShaderData>
#define ModuleInstance Instance<std::string, ModuleData>

class ShaderManager {
private:
	
	InstanceCache<std::string, ShaderData> GeometryShaderCache;		 // geometry passes
	InstanceCache<std::string, ShaderData> PassShaderCache;			 // post-processing, lighting, etc.
	InstanceCache<std::string, ModuleData> ShaderModuleCache;		 // post-processing, lighting, etc.

	int selectedGeometryShader;

	std::string defaultGeometryShader;
	std::string defaultPassShader;
public:
	ShaderManager() :
		GeometryShaderCache(10, false),
		PassShaderCache(10, false),
		ShaderModuleCache(10, false)
	{
		//Add shader and module construction
		GeometryShaderCache.addTypeInitialiser([](std::shared_ptr<ShaderData> data) {
			data->constructShader();
			});

		PassShaderCache.addTypeInitialiser([](std::shared_ptr<ShaderData> data) {
			data->constructShader();
			});

		ShaderModuleCache.addTypeInitialiser([](std::shared_ptr<ModuleData> data) {
			data->constructModule();
			});


		//Add shader and module destruction
		GeometryShaderCache.addTypeEndHandler([](std::shared_ptr<ShaderData> data) {
			data->shader.reset();
			});

		PassShaderCache.addTypeEndHandler([](std::shared_ptr<ShaderData> data) {
			data->shader.reset();
			});

		ShaderModuleCache.addTypeEndHandler([](std::shared_ptr<ModuleData> data) {
			data->module.reset();
			});

		selectedGeometryShader = -1;

		defaultGeometryShader = SIZE_MAX;
		defaultPassShader = SIZE_MAX;
	}

	template<typename ShaderType, typename... Args>
	void addGeometryShader(const std::string& name, Args&&... args) {
		// Capture constructor arguments by value or move
		auto argsTuple = std::make_tuple(std::forward<Args>(args)...);
		GeometryShaderCache.emplaceID(
			name,
			ShaderData(
				name,
				[argsTuple]() -> std::shared_ptr<BaseShader> {
					return std::apply([](auto&&... unpackedArgs) {
						return std::make_shared<ShaderType>(std::forward<decltype(unpackedArgs)>(unpackedArgs)...);
					}, argsTuple);
				}
			)
		);
	}

	template<typename ShaderType, typename... Args>
	void addPassShader(const std::string& name, Args&&... args) {
		// Capture constructor arguments by value or move
		auto argsTuple = std::make_tuple(std::forward<Args>(args)...);
		PassShaderCache.emplaceID(
			name,
			ShaderData(
				name,
				[argsTuple]() -> std::shared_ptr<BaseShader> {
					return std::apply([](auto&&... unpackedArgs) {
						return std::make_shared<ShaderType>(std::forward<decltype(unpackedArgs)>(unpackedArgs)...);
						}, argsTuple);
				}
			)
		);
	}

	ShaderInstance getGeometryShader(const std::string& name) {
		ShaderInstance instance = GeometryShaderCache.getID(name);
		if (!instance.IsValid()) {
			throw std::runtime_error("Error: Geometry shader not found: " + name);
		}
		return instance;
	}

	ShaderInstance getPassShader(const std::string& name) {
		ShaderInstance instance = PassShaderCache.getID(name);
		if (!instance.IsValid()) {
			throw std::runtime_error("Error: Pass shader not found: " + name);
		}
		return instance;
	}

	ModuleInstance getShaderModuleID(const std::string& name) {
		ModuleInstance instance = ShaderModuleCache.getID(name);
		if (!instance.IsValid()) {
			throw std::runtime_error("Error: Shader Module not found: " + name);
		}
		return instance;
	}

	void setDefaultGeometryShader(const std::string& name) {
		if (GeometryShaderCache.hasID(name)) {
			defaultGeometryShader = name;
			return;
		}
		throw std::runtime_error("Error: " + name + " Shader does not exist!");
	}

	const std::string& getDefaultGeometryShaderName() {
		return defaultGeometryShader;
	}

	ModuleInstance getDefaultGeometryShader()
	{
		if (defaultGeometryShader != "") {
			return getShaderModuleID(defaultGeometryShader);
		}
		else {
			return ModuleInstance();
		}
	}

	void selectGeometryShaderImGui(std::shared_ptr<Material> mat) {
		const std::vector<const char*>& vecNames = GeometryShaderCache.getCharVec();
		
		if (ImGui::Combo("Select Geometry Shader: ", &selectedGeometryShader, vecNames.data(), vecNames.size())) {
			mat->shader = getGeometryShader(GeometryShaderCache.getCachedName(selectedGeometryShader));
		}
	}
};

#endif