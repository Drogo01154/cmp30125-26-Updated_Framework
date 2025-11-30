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


enum class DirtyModuleFlags : uint32_t {
	NONE = 0,
	CAMERA = 1 << 0,
	LIGHTSDATACHANGED = 1 << 1,
	LIGHTNUMCHANGED = 1 << 2,
	LIGHTPROJECTIONCHANGED = 1 << 3
};

// Bitwise OR
inline DirtyModuleFlags operator|(DirtyModuleFlags a, DirtyModuleFlags b) {
	return static_cast<DirtyModuleFlags>(static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
}

inline DirtyModuleFlags& operator|=(DirtyModuleFlags& a, DirtyModuleFlags b) {
	a = a | b;
	return a;
}

// Bitwise AND
inline DirtyModuleFlags operator&(DirtyModuleFlags a, DirtyModuleFlags b) {
	return static_cast<DirtyModuleFlags>(static_cast<uint32_t>(a) & static_cast<uint32_t>(b));
}

inline DirtyModuleFlags& operator&=(DirtyModuleFlags& a, DirtyModuleFlags b) {
	a = a & b;
	return a;
}

// Bitwise NOT
inline DirtyModuleFlags operator~(DirtyModuleFlags a) {
	return static_cast<DirtyModuleFlags>(~static_cast<uint32_t>(a));
}



struct ModuleData {
	ModuleData(const std::string& name, DirtyModuleFlags checkFlags, std::function<std::shared_ptr<BaseShaderModule>()> constructModule) :
		name(name), checkFlags(checkFlags), constructModule(constructModule) {
	}
	std::string name;
	// Factory function that creates the shader
	std::function<std::shared_ptr<BaseShaderModule>()> constructModule;
	std::shared_ptr<BaseShaderModule> module;
	DirtyModuleFlags checkFlags;
};

#define ShaderInstance Instance<std::string, ShaderData>
#define ModuleInstance Instance<std::string, ModuleData>


class ShaderManager {
public:
	ShaderManager();

	void SetModuleDirtyflags(DirtyModuleFlags flag) { flags |= flag; }
	void clearModuleDirtyflags(DirtyModuleFlags flag) { flags &= ~flag; }
	bool isModuleDirtyflagSet(DirtyModuleFlags flag) const { return (flags & flag) != DirtyModuleFlags::NONE; }
	void ResetModuleDirtyflags() { flags = DirtyModuleFlags::NONE; }


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
			), false
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
			), false
		);
	}

	template<typename ModuleType, typename... Args>
	void AddShaderModule(const std::string& name, DirtyModuleFlags checkFlags, Args&&... args) {
		// Capture constructor arguments by value or move
		auto argsTuple = std::make_tuple(std::forward<Args>(args)...);
		ShaderModuleCache.emplaceID(
			name,
			ModuleData(
				name,
				checkFlags,
				[argsTuple]() -> std::shared_ptr<BaseShaderModule> {
					return std::apply([](auto&&... unpackedArgs) {
						return std::make_shared<ModuleType>(std::forward<decltype(unpackedArgs)>(unpackedArgs)...);
						}, argsTuple);
				}
			), false
		);
	}

	ShaderInstance getGeometryShader(const std::string& name);

	ShaderInstance getPassShader(const std::string& name);

	ModuleInstance getShaderModuleID(const std::string& name);

private:

	InstanceCache<std::string, ShaderData> GeometryShaderCache;		 // geometry passes
	InstanceCache<std::string, ShaderData> PassShaderCache;			 // post-processing, lighting, etc.
	InstanceCache<std::string, ModuleData> ShaderModuleCache;		 // post-processing, lighting, etc.

	DirtyModuleFlags flags;
};

#endif