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
	MaterialManager(ShaderManager* shaderManager, TextureManager* textureManager) : 
		materialCache(10, false),
		shaderManager(shaderManager), 
		textureManager(textureManager) {
		materialCache.addTypeInitialiser([&](std::shared_ptr<Material> mat) {
			if (mat->shaderName != "") {
				mat->shader = shaderManager->getGeometryShader(mat->shaderName);
			}
			if (mat->textureString != L"") {
				mat->texture = textureManager->getTexture(mat->textureString);
			}
			});

		materialCache.addTypeEndHandler([](std::shared_ptr<Material> mat) {
			if (mat->shader.IsValid()) {
				mat->shader = ShaderInstance();
			}
			if (mat->texture.IsValid()) {
				mat->texture = TextureInstance();
			}
			});

		
		selectedMaterial = -1;
		selectedTexture = -1;
		createMaterial("Default");
	};

	//Gets material ID from name
	MaterialInstance getMaterial(const std::string& name) {
		MaterialInstance inst = materialCache.getID(name);
		if (!inst.IsValid()) {
			throw std::runtime_error("Error: material does not exist at: " + name);
		}
		return inst;
	}

	//Renames a given material
	bool renameMaterial(const std::string& oldName, const std::string& newName, bool selectedMaterial = false) {
		if (materialCache.changeID(oldName, newName)) {
			const std::vector<std::string>& stringCache = materialCache.getCachedStringVec(false);
			for (int i = 0; i < stringCache.size(); ++i) {
				if (stringCache[i] == newName) {
					selectedMaterial = i;
					return true;
				}
			}
			throw std::runtime_error("Error: somehow name not in cache at: " + newName);
			return false;
		}
		return false;
	}


	//Creates material
	inline void createMaterial(const std::string& name) {
		if (name == "" || name == "Default") return;
		Material mat;

		mat.MaterialName = name;
		mat.baseColour = XMFLOAT4(1.f, 1.f, 1.f, 1.f);
		mat.specularColour = XMFLOAT4(1.f, 1.f, 1.f, 1.f);
		mat.specularPower = 32.f;
		mat.textureString = L"brick1";
		mat.shaderName = shaderManager->getDefaultGeometryShaderName();

		materialCache.emplaceID(name, std::move(mat));
	}

	void updateSelectedMaterial() {
		if (selectedMaterial >= 0) {

		}
	}

	//Updates what material is selected
	void updateSelectedMaterial() {
		if (selectedMaterial >= 0) {
			//Get ID of newly selected material
			size_t ID = materialNamesToIDs[std::string(materialNames[selectedMaterial])];

			//Get newly selected material
			std::shared_ptr<Material> newMat = materialMap.getID(ID);

			//Get materials name
			const std::string& matName = newMat->MaterialName;

			//Update input buffer to current name
			strncpy_s(nameInput, matName.c_str(), sizeof(nameInput));

			//Get vector image strings from file handler
			const std::vector<const char*>* imageList = FileHandler::get().getImageList();
			selectedTexture = -1;
			if (newMat->textureString != L"") {
				//Find currently selected image from list
				for (int i = 0; i < imageList->size(); i++) {
					std::string testString = Converters::convert_from_wstring(newMat->textureString);
					if (std::string((*imageList)[i]) == testString) {
						selectedTexture = i;
						break;
					}
				}
			}
		}
		else {
			// Clear buffer
			nameInput[0] = '\0';
			selectedTexture = -1;
		}
	}

	inline bool deleteMaterial(const std::string& name) {
		if (selectedMaterial >= 0) {
			if (name == materialCache.getCachedStringVec()[selectedMaterial]) 
			{ 
				selectedMaterial = -1;
				nameInput[0] = '\0';
				selectedTexture = -1;
			}
		}
		materialCache.removeID(name);
	}

	inline void deleteAllMaterials() {
		selectedMaterial = -1;
		nameInput[0] = '\0';
		selectedTexture = -1;
		materialCache.clear();
	}

	inline void imGuiRender() {

		if (ImGui::Combo("Select Material", &selectedMaterial, materialNames.data(), materialNames.size())) {
			updateSelectedMaterial();
		}

		if (selectedMaterial >= 0) {
			//Get selected materials ID
			size_t ID = materialNamesToIDs[std::string(materialNames[selectedMaterial])];
			//Get material from ID
			std::shared_ptr<Material> mat = materialMap.getID(ID);
			//Output materials Name
			if (ImGui::InputText("Material Name", nameInput, sizeof(nameInput),
				ImGuiInputTextFlags_EnterReturnsTrue)) {
				// This block executes when the user presses Enter
				std::string newName(nameInput);
				std::string oldName = mat->MaterialName; // current material name

				if (!newName.empty() && newName != oldName) {
					renameMaterial(oldName, newName, true); // rename material and preserve selection
				}
			}
			if(ImGui::TreeNode("Material: Colour")) {
				ImGui::ColorPicker4("Material Albedo: ", &mat->baseColour.x);
				ImGui::TreePop();
			}
			if (ImGui::TreeNode("Material: Specular")) {
				ImGui::ColorPicker4("Material Specular Colour: ", &mat->specularColour.x);
				ImGui::SliderFloat("Specular Power: ## 0", &mat->specularPower, 1.f, 1000.f);
				ImGui::TreePop();
			}
			shaderManager->selectGeometryShaderImGui(mat);
			const std::vector<const char*>* imageList = FileHandler::get().getImageList();
			if (ImGui::Combo("Select Material Texture: ", &selectedTexture, imageList->data(), imageList->size())) {
				bool inUse = (mat->liveUsers > 0);

				if (inUse) {
					//Release old texture if not used by other material or object
					textureManager->checkRemove(mat->textureString);
				}
				//Get new texture string
				mat->textureString = Converters::convert_to_wstring(std::string((*imageList)[selectedTexture]));
				if (inUse) {
					mat->texture = textureManager->getTexture(mat->textureString);
				}
				else {
					mat->texture = nullptr;
				}
			}
		}
	}

	void to_json(nlohmann::json& j) {
		j = nlohmann::json::array();
		materialMap.forEach([&](size_t ID, std::shared_ptr<Material> mat) {
			nlohmann::json matJson;
			matJson["OldID"] = static_cast<uint64_t>(ID);
			matJson["BaseColour"] = mat->baseColour;
			matJson["MaterialName"] = mat->MaterialName;
			matJson["Texture"] = Converters::convert_from_wstring(mat->textureString);
			matJson["ShaderID"] = static_cast<uint64_t>(mat->ShaderID);
			matJson["SpecularColour"] = mat->specularColour;
			matJson["SpecularPower"] = mat->specularPower;
			j.push_back(matJson);
			});
		
	}
	void from_json(const nlohmann::json& j, std::unordered_map<size_t, size_t>* newMaterialIDMap) {
		deleteAllMaterials();
		for (auto matJson : j) {
			auto newMat = materialMap.emplaceID(Material());
			size_t oldID = static_cast<size_t>(matJson.at("OldID").get<uint64_t>());
			newMaterialIDMap->emplace(oldID, newMat.first);
			newMat.second->baseColour = matJson.at("BaseColour").get<XMFLOAT4>();
			newMat.second->MaterialName = matJson.at("MaterialName").get<std::string>();
			newMat.second->textureString = Converters::convert_to_wstring(matJson.at("Texture").get<std::string>());
			newMat.second->ShaderID = static_cast<size_t>(matJson.at("ShaderID").get<uint64_t>());
			newMat.second->specularColour = matJson.at("SpecularColour").get<XMFLOAT4>();
			newMat.second->specularPower = matJson.at("SpecularPower").get<float>();
			materialNamesToIDs.emplace(newMat.second->MaterialName, newMat.first);
		}
		resizeMaterialNames(false, "");
	}

private:
	ShaderManager* shaderManager;
	TextureManager* textureManager;
	InstanceCache<std::string, Material> materialCache;
	std::unordered_map<std::string, size_t> materialNamesToIDs;
	std::vector<const char*> materialNames;

	int selectedMaterial;
	int selectedTexture;
	char nameInput[256] = "";
};

#endif