#include "ShaderManager.h"
#include "Material.h"

//Constructor
ShaderManager::ShaderManager() :
	GeometryShaderCache(10, false),
	PassShaderCache(10, false),
	ShaderModuleCache(10, false)
{
	//Make it so geometry shader constructed on first instance initialisation
	GeometryShaderCache.addTypeInitialiser([](ShaderData* data) {
		data->shader = data->constructShader();
		});

	//Make it so pass shader constructed on first instance initialisation
	PassShaderCache.addTypeInitialiser([](ShaderData* data) {
		data->shader = data->constructShader();
		});

	//Make it so module constructed on first instance initialisation
	ShaderModuleCache.addTypeInitialiser([](ModuleData* data) {
		data->module = data->constructModule();
		});


	//Make it so geometry shader destroyed on last instance destruction
	GeometryShaderCache.addTypeEndHandler([](ShaderData* data) {
		data->shader.reset();
		});
	//Make it so pass shader destroyed on last instance destruction
	PassShaderCache.addTypeEndHandler([](ShaderData* data) {
		data->shader.reset();
		});
	//Make it so moule destroyed on last instance destruction
	ShaderModuleCache.addTypeEndHandler([](ModuleData* data) {
		data->module.reset();
		});
}

//Getter for geoemtry shader instances
ShaderInstance ShaderManager::getGeometryShader(const std::string& name) {
	ShaderInstance instance = GeometryShaderCache.getInstance(name);
	if (!instance.IsValid()) {
		throw std::runtime_error("Error: Geometry shader not found: " + name);
	}
	return instance;
}

//Getter for pass shader instances
ShaderInstance ShaderManager::getPassShader(const std::string& name) {
	ShaderInstance instance = PassShaderCache.getInstance(name);
	if (!instance.IsValid()) {
		throw std::runtime_error("Error: Pass shader not found: " + name);
	}
	return instance;
}

//Getter for module instances
ModuleInstance ShaderManager::getShaderModuleID(const std::string& name) {
	ModuleInstance instance = ShaderModuleCache.getInstance(name);
	if (!instance.IsValid()) {
		throw std::runtime_error("Error: Shader Module not found: " + name);
	}
	return instance;
}