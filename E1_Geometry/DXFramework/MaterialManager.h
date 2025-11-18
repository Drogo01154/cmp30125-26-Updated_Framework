#pragma once

#ifndef _MATERIALMANAGER_H_
#define _MATERIALMANAGER_H_

#include "Material.h"
#include "IDMap.h"
#include "ShaderManager.h"
#include "Converters.h"

class MaterialManager {
public:
	//Constructor
	MaterialManager(ShaderManager* shaderManager, TextureManager* textureManager) : shaderManager(shaderManager), textureManager(textureManager) {
		selectedMaterial = -1;
		selectedTexture = -1;
		createMaterial("Default");
	};

	//Gets material ID from name
	size_t getMaterialID(const std::string& name) {
		auto it = materialNamesToIDs.find(name);
		if (it != materialNamesToIDs.end()) {
			return it->second;
		}
		throw std::runtime_error("Error: material does not exist at: " + name);
	}

	//Renames a given material
	void renameMaterial(const std::string& oldName, const std::string& newName, bool selectedMaterial = false) {
		if (oldName == "Default") { return; }
		//Check if new name already in use
		if (materialNamesToIDs.find(newName) != materialNamesToIDs.end()) {
			return;
		}
		auto it = materialNamesToIDs.find(oldName);
		if (it == materialNamesToIDs.end()) {
			throw std::runtime_error("Error: Old material name does not exist: " + oldName);
		}
		//Get ID of material
		size_t ID = it->second;
		materialNamesToIDs.erase(it);
		materialNamesToIDs[newName] = ID;
		materialMap.getID(ID)->MaterialName = newName;
		resizeMaterialNames(true, newName);
	}

	//Gets a chosen material by its ID
	std::shared_ptr<Material> getMaterial(size_t ID) {
		//Attempt get material
		std::shared_ptr<Material> mat = materialMap.getID(ID);
		if (mat) { // if material in map
			if (mat->liveUsers == 0) { // if no users yet load data
				mat->texture = textureManager->getTexture(mat->textureString);
				shaderManager->AddGeometryShaderInstanceReference(mat->ShaderID); // Tracks that this shader is going to be used so instantiates it if not already
			}
		}
		else {
			throw std::runtime_error("Error: Material does not exist at ID " + std::to_string(ID));
		}
		mat->liveUsers++;
		return mat;
	}

	//Function for removing a reference to the material being used
	void removeMaterialReference(size_t ID) {
		std::shared_ptr<Material> mat = materialMap.getID(ID);
		if (!mat) {
			throw std::runtime_error("Error: Material does not exist at ID " + std::to_string(ID));
		}
		
		if (mat->liveUsers == 0) {	// Prevent underflow by clamping to zero
			return;
		}
		mat->liveUsers--;
		if (mat->liveUsers == 0) { // If material only refernced in this class and not used anywhere else
			mat->texture = nullptr;
			shaderManager->DeReferenceGeometryShader(mat->ShaderID); // Tell shader manager geometry shader no longer in use
			textureManager->checkRemove(mat->textureString); // Tell texture manager texture no longer being used here
		}
	}


	//Resized vector of material names for combo list
	inline void resizeMaterialNames(bool preserveSelection = false, const std::string& selectedName = "") {
		materialNames.clear();
		int newSelectedIndex = -1;
		size_t iterator = 0;
		for (auto& it : materialNamesToIDs) {
			if (preserveSelection && it.first == selectedName) {
				newSelectedIndex = static_cast<int>(iterator);
			}
			materialNames.push_back(it.first.c_str());
			iterator++;
		}

		if (preserveSelection) {
			selectedMaterial = (newSelectedIndex != -1) ? newSelectedIndex : -1;
		}
		else if (selectedMaterial >= static_cast<int>(materialNames.size())) {
			selectedMaterial = -1; // reset if out of bounds
		}
	}

	//Creates material
	inline size_t createMaterial(const std::string& name) {
		std::pair<size_t, std::shared_ptr<Material>> materialData = materialMap.emplaceID(Material());
		materialData.second->MaterialName = name;
		materialData.second->liveUsers = 0;
		materialData.second->baseColour = XMFLOAT4(1.f, 1.f, 1.f, 1.f);
		materialData.second->specularColour = XMFLOAT4(1.f, 1.f, 1.f, 1.f);
		materialData.second->specularPower = 32.f;
		materialData.second->ShaderID = shaderManager->getDefaultGeometryShader();
		materialData.second->textureString = L"brick1";
		materialData.second->texture = textureManager->getTexture(materialData.second->textureString);
		resizeMaterialNames();
		return materialData.first;
	}

	//Updates what material is selected
	void UpdateSelectedMaterial() {
		if (selectedMaterial >= 0) {
			//Get ID of newly selected material
			size_t ID = materialNamesToIDs[std::string(materialNames[selectedMaterial])];

			//Get newly selected material
			std::shared_ptr<Material> newMat = materialMap.getID(ID);

			//Get materials name
			const std::string& matName = newMat->MaterialName;

			//Update input buffer to current name
			strncpy(nameInput, matName.c_str(), sizeof(nameInput));

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
		bool hasUsers = false;
		auto it = materialNamesToIDs.find(name);	// Find material
		if (it != materialNamesToIDs.end()) { // Material exists!
			
			std::string selectedName = "";
			bool nameSelected = selectedMaterial >= 0;

			if (nameSelected) {
				selectedName = std::string(materialNames[selectedMaterial]);
				if (selectedName == name) {
					selectedMaterial = -1;
					nameSelected = false;
				}
			}
			size_t ID = materialNamesToIDs[name];


			auto mat = materialMap.getID(ID);
			hasUsers = mat->liveUsers > 0;
			if (hasUsers) {
				if (mat->textureString != L"") {
					mat->texture = nullptr;
					textureManager->checkRemove(mat->textureString);
				}

				shaderManager->DeReferenceGeometryShader(mat->ShaderID);
			}
		

			materialNamesToIDs.erase(name);
			materialMap.eraseID(ID);

			resizeMaterialNames(nameSelected, selectedName);
		}
		else {
			throw std::runtime_error("Error: material does not exist at: " + name);
		}
		return hasUsers;
	}

	inline bool deleteMaterial(size_t ID) {
		bool hasUsers = false;
		if (selectedMaterial >= 0) {
			std::string selectedName = std::string(materialNames[selectedMaterial]);
		}
		;
		if (auto mat = materialMap.getID(ID)) { // Find material
			
			std::string selectedName = "";
			bool nameSelected = selectedMaterial >= 0;

			hasUsers = mat->liveUsers > 0;

			if (hasUsers) {
				if (mat->textureString != L"") {
					mat->texture = nullptr;
					textureManager->checkRemove(mat->textureString);
				}

				shaderManager->DeReferenceGeometryShader(mat->ShaderID);
			}

			if (nameSelected) {
				selectedName = std::string(materialNames[selectedMaterial]);
				if (selectedName == mat->MaterialName) {
					selectedMaterial = -1;
					nameSelected = false;
				}
			}
		
			materialNamesToIDs.erase(mat->MaterialName);
			materialMap.eraseID(ID);

			resizeMaterialNames(nameSelected, selectedName);

		}
		else {
			throw std::runtime_error("Error: material does not exist at ID: " + std::to_string(ID));
		}
		return hasUsers;
	}

	inline void deleteAllMaterials() {
		materialMap.forEach([&](size_t ID, std::shared_ptr<Material> mat) {
			if (mat->liveUsers > 0) {
				if (mat->textureString != L"") {
					mat->texture = nullptr;
					textureManager->checkRemove(mat->textureString);
				}

				shaderManager->DeReferenceGeometryShader(mat->ShaderID);
				}
			});
		materialNamesToIDs.clear();
		materialMap.clear();
		materialNames.clear();
		selectedMaterial = -1;
		// Clear buffer
		nameInput[0] = '\0';
		selectedTexture = -1;
	}

	inline void ImGuiRender() {

		if (ImGui::Combo("Select Material", &selectedMaterial, materialNames.data(), materialNames.size())) {
			UpdateSelectedMaterial();
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
			shaderManager->SelectGeometryShaderImGui(mat);
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
	IDMap<Material> materialMap;
	std::unordered_map<std::string, size_t> materialNamesToIDs;
	std::vector<const char*> materialNames;

	int selectedMaterial;
	int selectedTexture;
	char nameInput[256] = "";
};

#endif