#pragma once

#ifndef _SHADERMANAGER_H_
#define _SHADERMANAGER_H_

#include "BaseShader.h"
#include "BaseShaderModule.h"
#include "IDMap.h"
#include <vector>
#include "Material.h"
#include <functional>

struct ShaderData {
	std::string name;
	size_t ID;
	bool loaded;
	size_t references;	// Number of objects activelt using shader

	// Factory function that creates the shader
	std::function<std::shared_ptr<BaseShader>()> constructShader;
	std::shared_ptr<BaseShader> shader;
};

struct ModuleData {
	std::string name;
	size_t ID;
	bool loaded;
	size_t references;
	// Factory function that creates the shader
	std::function<std::shared_ptr<BaseShaderModule>()> constructModule;
	std::shared_ptr<BaseShaderModule> module;
};

class ShaderManager {
private:
	IDMap<ShaderData> GeometryShaders;  // geometry passes
	IDMap<ShaderData> PassShaders;      // post-processing, lighting, etc.
	IDMap<ModuleData> ShaderModules;      // post-processing, lighting, etc.

	std::unordered_map<std::string, size_t> geometryNameToIDs;	// Map geomemtry shader names -> IDs, Also if loaded
	std::unordered_map<std::string, size_t> passNameToIDs;		// Map pass shader names -> IDs, Also if loaded
	std::unordered_map<std::string, size_t> moduleNameToIDs;		// Map module naes -> IDs, Also if loaded

	std::vector<const char*> geometryShaderNames;

	int selectedGeometryShader;
	size_t defaultGeometryShader;
	size_t defaultPassShader;
public:
	ShaderManager() 
	{
		selectedGeometryShader = -1;

		defaultGeometryShader = SIZE_MAX;
		defaultPassShader = SIZE_MAX;
	}

	void rebuildGeometryShaderNames() {
		geometryShaderNames.clear();
		for (auto it : geometryNameToIDs) {
			geometryShaderNames.push_back(it.first.c_str());
		}
		while (selectedGeometryShader >= geometryShaderNames.size()) { selectedGeometryShader--; }
	}

	template<typename ShaderType, typename... Args>
	std::shared_ptr<ShaderData> addGeometryShader(const std::string& name, Args&&... args) {
		auto it = geometryNameToIDs.find(name);
		if (it == geometryNameToIDs.end()) {  // shader does not exist yet

			auto shader = std::make_shared<ShaderData>();
			shader->name = name;
			shader->loaded = false;
			shader->references = 0;

			auto pair = GeometryShaders.emplaceID(shader);
			shader->ID = pair.first;

			// Capture constructor arguments by value or move
			auto argsTuple = std::make_tuple(std::forward<Args>(args)...);
			shader->constructShader = [argsTuple]() -> std::shared_ptr<BaseShader> {
				return std::apply([](auto&&... unpackedArgs) {
					return std::make_shared<ShaderType>(std::forward<decltype(unpackedArgs)>(unpackedArgs)...);
					}, argsTuple);	
				};

			geometryNameToIDs.emplace(name, shader->ID);
			rebuildGeometryShaderNames();

			return shader;

		}
		else {
			throw std::runtime_error("Error: " + name + " Shader already exists!");
		}
	}

	template<typename ShaderType, typename... Args>
	std::shared_ptr<ShaderData> addPassShader(const std::string& name, Args&&... args) {
		auto it = passNameToIDs.find(name);
		if (it == passNameToIDs.end()) {  // shader does not exist yet

			auto shader = std::make_shared<ShaderData>();
			shader->name = name;
			shader->loaded = false;
			shader->references = 0;

			auto pair = PassShaders.emplaceID(shader);
			shader->ID = pair.first;

			// Capture constructor arguments by value or move
			auto argsTuple = std::make_tuple(std::forward<Args>(args)...);
			shader->constructShader = [argsTuple]() -> std::shared_ptr<BaseShader> {
				return std::apply([](auto&&... unpackedArgs) {
					return std::make_shared<ShaderType>(std::forward<decltype(unpackedArgs)>(unpackedArgs)...);
					}, argsTuple);
				};

			passNameToIDs.emplace(name, shader->ID);

			return shader;

		}
		else {
			throw std::runtime_error("Error: " + name + " Shader already exists!");
		}
	}

	template<typename ModuleType, typename... Args>
	std::shared_ptr<ModuleData> addShaderModule(const std::string& name, Args&&... args) {
		auto it = moduleNameToIDs.find(name);
		if (it == moduleNameToIDs.end()) {  // module does not exist yet

			auto module = std::make_shared<ModuleData>();
			module->name = name;
			module->loaded = false;
			module->references = 0;

			auto pair = ShaderModules.emplaceID(module);
			module->ID = pair.first;

			// Capture constructor arguments by value or move
			auto argsTuple = std::make_tuple(std::forward<Args>(args)...);
			module->constructModule = [argsTuple]() -> std::shared_ptr<BaseShaderModule> {
				return std::apply([](auto&&... unpackedArgs) {
					return std::make_shared<ModuleType>(std::forward<decltype(unpackedArgs)>(unpackedArgs)...);
					}, argsTuple);
				};

			// Add name ? ID mapping
			moduleNameToIDs.emplace(name, module->ID);

			return module;

		} else {
			throw std::runtime_error("Error: " + name + " Module already exists!");
		}
	}

	void AddGeometryShaderInstanceReference(size_t ID) {
		auto record = GeometryShaders.getID(ID);
		if (!record) {
			throw std::runtime_error("Error: Shader ID not found: " + std::to_string(ID));
		}

		if (!record->loaded) {
			record->shader = record->constructShader();
			record->loaded = true;
		}

		record->references++;
	}
		

	void AddPassShaderInstanceReference(size_t ID) {
		auto record = PassShaders.getID(ID);
		if (!record) {
			throw std::runtime_error("Error: Shader ID not found: " + std::to_string(ID));
		}
		
		if (!record->loaded) {
			record->shader = record->constructShader();
			record->loaded = true;
		}

		record->references++;
	}

	std::shared_ptr<BaseShaderModule> AddShaderModuleInstanceReference(size_t ID) {
		auto record = ShaderModules.getID(ID);

		if (!record) {
			throw std::runtime_error("Error: Shader Module ID not found: " + std::to_string(ID));
		}

		if (!record->loaded) {
			record->module = record->constructModule();
			record->loaded = true;
		}

		record->references++;
	}

	size_t GetGeometryShaderID(const std::string& name) {
		auto recordID = geometryNameToIDs.find(name);
		if (recordID != geometryNameToIDs.end()) {
			return recordID->second;
		}
		else {
			throw std::runtime_error("Error: Shader not found: " + name);
			return SIZE_MAX;
		}

	}

	size_t GetPassShaderID(const std::string& name) {
		auto recordID = passNameToIDs.find(name);
		if (recordID != passNameToIDs.end()) {
			return recordID->second;
		}
		else {
			throw std::runtime_error("Error: Shader not found: " + name);
			return SIZE_MAX;
		}
	}

	size_t getShaderModuleID(const std::string& name) {
		auto recordID = moduleNameToIDs.find(name);
		if (recordID != moduleNameToIDs.end()) {
			return recordID->second;
		}
		else {
			throw std::runtime_error("Error: Shader Module not found: " + name);
			return SIZE_MAX;
		}
	}

	std::shared_ptr<BaseShader> GetGeometryShader(size_t ID) {
		auto record = GeometryShaders.getID(ID);
		if (record) {
			if (record->loaded && record->shader) {
				return record->shader;
			}
			else {
				throw std::runtime_error("Error: Shader not loaded: " + std::to_string(ID));
			}
		}
		else {
			throw std::runtime_error("Error: Shader ID not found: " + std::to_string(ID));
		}
		return {};
	}

	std::shared_ptr<BaseShader> GetPassShader(size_t ID) {
		auto record = PassShaders.getID(ID);
		if (record) {
			if (record->loaded && record->shader) {
				return record->shader;
			}
			else {
				throw std::runtime_error("Error: Shader not loaded: " + std::to_string(ID));
			}
		}
		else {
			throw std::runtime_error("Error: Shader ID not found: " + std::to_string(ID));
		}
		return {};
	}

	std::shared_ptr<BaseShaderModule> getShaderModuleID(size_t ID) {
		auto record = ShaderModules.getID(ID);
		if (record) {
			if (record->loaded && record->module) {
				return record->module;
			}
			else {
				throw std::runtime_error("Error: Shader Module not loaded: " + std::to_string(ID));
			}
		}
		else {
			throw std::runtime_error("Error: Shader Module ID not found: " + std::to_string(ID));
		}
		return {};
	}

	void DeReferenceGeometryShader(size_t ID) {
		auto record = GeometryShaders.getID(ID);
		if (!record) {
			throw std::runtime_error("Error: Shader ID not found: " + std::to_string(ID));
		}
		record->references--;
		if (record->references == 0) {
			record->shader.reset();
			record->loaded = false;
		}
	}

	void DeReferencePassShader(size_t ID) {
		auto record = PassShaders.getID(ID);
		if (!record) {
			throw std::runtime_error("Error: Shader ID not found: " + std::to_string(ID));
		}
		record->references--;
		if (record->references == 0) {
			record->shader.reset();
			record->loaded = false;
		}
	}

	void DeReferenceShaderModule(size_t ID) {
		auto record = ShaderModules.getID(ID);

		if (!record) {
			throw std::runtime_error("Error: Shader Module ID not found: " + std::to_string(ID));
		}
		record->references--;
		//Delete module
		if (record->references == 0) {
			record->module.reset();
			record->loaded = false;
		}
	}

	void setDefaultGeometryShader(const std::string& name) {
		auto it = geometryNameToIDs.find(name);
		if (it != geometryNameToIDs.end()) {
			defaultGeometryShader = it->second;
			return;
		}
		throw std::runtime_error("Error: " + name + " Shader does not exist!");
	}

	size_t getDefaultGeometryShader() 
	{
		return defaultGeometryShader; 
	}

	void SelectGeometryShaderImGui(std::shared_ptr<Material> mat) {
		if (ImGui::Combo("Select Geometry Shader: ", &selectedGeometryShader, geometryShaderNames.data(), geometryShaderNames.size())) {
			DeReferenceGeometryShader(mat->ShaderID);
			mat->ShaderID = geometryNameToIDs[std::string(geometryShaderNames[selectedGeometryShader])];
			AddGeometryShaderInstanceReference(mat->ShaderID);
		}
	}
};

#endif