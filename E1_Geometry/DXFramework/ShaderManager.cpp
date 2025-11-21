#include "ShaderManager.h"
#include "Material.h"

ShaderManager::ShaderManager() :
	GeometryShaderCache(10, false),
	PassShaderCache(10, false),
	ShaderModuleCache(10, false)
{
	//Add shader and module construction
	GeometryShaderCache.addTypeInitialiser([](ShaderData* data) {
		data->constructShader();
		});

	PassShaderCache.addTypeInitialiser([](ShaderData* data) {
		data->constructShader();
		});

	ShaderModuleCache.addTypeInitialiser([](ModuleData* data) {
		data->constructModule();
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

	selectedGeometryShader = -1;

	defaultGeometryShader = SIZE_MAX;
	defaultPassShader = SIZE_MAX;
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

void ShaderManager::setDefaultGeometryShader(const std::string& name) {
	if (GeometryShaderCache.hasID(name)) {
		defaultGeometryShader = name;
		return;
	}
	throw std::runtime_error("Error: " + name + " Shader does not exist!");
}

const std::string& ShaderManager::getDefaultGeometryShaderName() {
	return defaultGeometryShader;
}

ShaderInstance ShaderManager::getDefaultGeometryShader()
{
	if (defaultGeometryShader != "") {
		return getGeometryShader(defaultGeometryShader);
	}
	else {
		return ShaderInstance();
	}
}

void ShaderManager::selectGeometryShaderImGui(Material* mat) {
	const std::vector<const char*>& vecNames = GeometryShaderCache.getCharVec();

	if (ImGui::Combo("Select Geometry Shader: ", &selectedGeometryShader, vecNames.data(), vecNames.size())) {
		mat->shader = getGeometryShader(GeometryShaderCache.getCachedName(selectedGeometryShader));
	}
}