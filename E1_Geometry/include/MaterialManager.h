#pragma once

#ifndef _MATERIALMANAGER_H_
#define _MATERIALMANAGER_H_

#include "Material.h"
#include "ShaderManager.h"
#include "Converters.h"
#include "SerializationHelpers.h"

class MaterialManager {
public:
	//Constructor
	MaterialManager(ShaderManager* shaderManager, TextureManager* textureManager);

	//Gets material ID from name
	MaterialInstance getMaterialInstance(const std::string& name);

	bool renameMaterial(const std::string& oldName, const std::string& newName, bool selectedMaterial = false);

	//Creates material
	void createMaterial(const std::string& name);

	void updateSelectedMaterial();

	bool deleteMaterial(const std::string& name);

	void deleteAllMaterials();

	void imGuiRender();

	void to_json(nlohmann::json& j);
	void from_json(const nlohmann::json& j);
private:
	ShaderManager* shaderManager;
	TextureManager* textureManager;
	InstanceCache<std::string, Material> materialCache;
	std::unordered_map<std::string, size_t> materialNamesToIDs;
	std::vector<const char*> materialNames;

	int selectedMaterial;
	int selectedTexture;
	int selectedHeightMapTexture;
	char nameInput[256] = "";
};

#endif