#pragma once

#ifndef _SHADERMANAGER_H_
#define _SHADERMANAGER_H_

#include "BaseShader.h"
#include "BaseShaderModule.h"
#include "InstanceCache.h"
#include <functional>
#include <unordered_set>

struct Material;

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
public:
	ShaderManager();

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

	template<typename ModuleType, typename... Args>
	void AddShaderModule(const std::string& name, Args&&... args) {
		// Capture constructor arguments by value or move
		auto argsTuple = std::make_tuple(std::forward<Args>(args)...);
		ShaderModuleCache.emplaceID(
			name,
			ModuleData(
				name,
				[argsTuple]() -> std::shared_ptr<BaseShaderModule> {
					return std::apply([](auto&&... unpackedArgs) {
						return std::make_shared<ModuleType>(std::forward<decltype(unpackedArgs)>(unpackedArgs)...);
						}, argsTuple);
				}
			)
		);
	}

	ShaderInstance getGeometryShader(const std::string& name);

	ShaderInstance getPassShader(const std::string& name);

	ModuleInstance getShaderModuleID(const std::string& name);

	void setDefaultGeometryShader(const std::string& name);

	const std::string& getDefaultGeometryShaderName(); 

	ShaderInstance getDefaultGeometryShader();

	bool selectGeometryShaderImGui(Material* mat);

	void AddHeightMapShaderName(const std::string& shader);
	void RemoveHeightMapShaderName(const std::string& shader);
	bool isHeightMapShader(const std::string& shader);
private:

	InstanceCache<std::string, ShaderData> GeometryShaderCache;		 // geometry passes
	InstanceCache<std::string, ShaderData> PassShaderCache;			 // post-processing, lighting, etc.
	InstanceCache<std::string, ModuleData> ShaderModuleCache;		 // post-processing, lighting, etc.

	std::unordered_set<std::string> HeightMapShaders;

	int selectedGeometryShader;

	std::string defaultGeometryShader;
	std::string defaultPassShader;
};

#endif