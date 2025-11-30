#include "ShaderManager.h"
#include "Material.h"

ShaderManager::ShaderManager() :
	GeometryShaderCache(10, false),
	PassShaderCache(10, false),
	ShaderModuleCache(10, false)
{
	//Add shader and module construction
	GeometryShaderCache.addTypeInitialiser([](ShaderData* data) {
		data->shader = data->constructShader();
		});

	PassShaderCache.addTypeInitialiser([](ShaderData* data) {
		data->shader = data->constructShader();
		});

	ShaderModuleCache.addTypeInitialiser([](ModuleData* data) {
		data->module = data->constructModule();
		});


	//Add shader and module destruction
	GeometryShaderCache.addTypeEndHandler([](ShaderData* data) {
		data->shader.reset();
		});

	PassShaderCache.addTypeEndHandler([](ShaderData* data) {
		data->shader.reset();
		});

	ShaderModuleCache.addTypeEndHandler([](ModuleData* data) {
		data->module.reset();
		});
}

ShaderInstance ShaderManager::getGeometryShader(const std::string& name) {
	ShaderInstance instance = GeometryShaderCache.getInstance(name);
	if (!instance.IsValid()) {
		throw std::runtime_error("Error: Geometry shader not found: " + name);
	}
	return instance;
}

ShaderInstance ShaderManager::getPassShader(const std::string& name) {
	ShaderInstance instance = PassShaderCache.getInstance(name);
	if (!instance.IsValid()) {
		throw std::runtime_error("Error: Pass shader not found: " + name);
	}
	return instance;
}

ModuleInstance ShaderManager::getShaderModuleID(const std::string& name) {
	ModuleInstance instance = ShaderModuleCache.getInstance(name);
	if (!instance.IsValid()) {
		throw std::runtime_error("Error: Shader Module not found: " + name);
	}
	return instance;
}