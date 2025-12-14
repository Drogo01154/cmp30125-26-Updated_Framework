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

	const std::vector<std::string>* getMaterialStrings();
	const std::vector<const char*>* getMatrialsChars();


	void imGuiRender();

	void to_json(nlohmann::json& j);
	void from_json(const nlohmann::json& j);
private:
	ShaderManager* shaderManager;
	TextureManager* textureManager;
	InstanceCache<std::string, Material> materialCache;

	int selectedMaterial;
	int selectedDiffuseTexture;
	int selectedEmissiveTexture;
	int selectedHeightMapTexture;

	char newNameInput[256] = "";
	char existingNameInput[256] = "";
};

#endif