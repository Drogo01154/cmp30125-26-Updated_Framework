#include "MaterialManager.h"

//Constructor
MaterialManager::MaterialManager(ShaderManager* shaderManager, TextureManager* textureManager) :
	materialCache(10, false),
	shaderManager(shaderManager),
	textureManager(textureManager) {
	materialCache.addTypeInitialiser([&](Material* mat) {
		if (!mat->shaderName.empty()) {
			mat->shader = this->shaderManager->getGeometryShader(mat->shaderName);
		}
		if (!mat->textureString.empty()) {
			mat->texture = this->textureManager->getTexture(mat->textureString);
		}
		if (mat->HeightMapData != nullptr && !mat->HeightMapData->HeightTextureString.empty()) {
			mat->HeightMapData->HeightTexture = this->textureManager->getTexture(mat->HeightMapData->HeightTextureString);
		}
	});

	materialCache.addTypeEndHandler([](Material* mat) {
		if (mat->shader.IsValid()) {
			mat->shader = ShaderInstance();
		}
		if (mat->texture.IsValid()) {
			mat->texture = TextureInstance();
		}
		if (mat->HeightMapData != nullptr && mat->HeightMapData->HeightTexture.IsValid()) {
			mat->HeightMapData->HeightTexture = TextureInstance();
		}
	});


	selectedMaterial = -1;
	selectedTexture = -1;
	selectedHeightMapTexture = -1;
};

//Gets material ID from name
MaterialInstance MaterialManager::getMaterialInstance(const std::string& name) {
	MaterialInstance inst = materialCache.tryGetInstance(name);
	if (!inst.IsValid()) {
		throw std::runtime_error("Error: material does not exist at: " + name);
	}
	return inst;
}

//Renames a given material
bool MaterialManager::renameMaterial(const std::string& oldName, const std::string& newName, bool selectedMaterial) {
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
void MaterialManager::createMaterial(const std::string& name) {
	if (name == "" || materialCache.hasID(name)) {
		return;
	}
	Material mat;

	mat.MaterialName = name;
	mat.baseColour = XMFLOAT4(1.f, 1.f, 1.f, 1.f);
	mat.specularColour = XMFLOAT4(1.f, 1.f, 1.f, 1.f);
	mat.specularPower = 32.f;
	mat.textureString = L"";
	mat.shaderName = shaderManager->getDefaultGeometryShaderName();
	mat.HeightMapData = nullptr;

	materialCache.emplaceID(name, std::move(mat), false);
}

void MaterialManager::updateSelectedMaterial() {

	// Reset state by default
	nameInput[0] = '\0';
	selectedTexture = -1;
	selectedHeightMapTexture = -1;

	if (selectedMaterial < 0) {
		return;
	}

	const auto& vec = materialCache.getCachedStringVec();
	if (selectedMaterial >= vec.size()) {
		selectedMaterial = -1;
		return;
	}

	const std::string& key = vec[selectedMaterial];
	auto mat = materialCache.tryGetValue(key);
	if (!mat) {
		selectedMaterial = -1;
		return;
	}

	// Update name buffer
	strncpy_s(nameInput, mat->MaterialName.c_str(), sizeof(nameInput));

	// Texture handling
	if (!mat->textureString.empty()) {
		const auto& imageList = *FileHandler::get().getImageList();

		std::string textureName = Converters::convert_from_wstring(mat->textureString);

		// Find matching texture
		for (int i = 0; i < imageList.size(); i++) {
			if (textureName == imageList[i]) {
				selectedTexture = i;
				break;
			}
		}
	}

	//Height map data handling
	if (mat->HeightMapData != nullptr) {
		if (!mat->HeightMapData->HeightTextureString.empty()) {
			const auto& imageList = *FileHandler::get().getImageList();
			std::string textureName = Converters::convert_from_wstring(mat->HeightMapData->HeightTextureString);

			// Find matching texture
			for (int i = 0; i < imageList.size(); i++) {
				if (textureName == imageList[i]) {
					selectedHeightMapTexture = i;
					break;
				}
			}
		}
	}
}

bool MaterialManager::deleteMaterial(const std::string& name) {
	bool selectedName = ((selectedMaterial >= 0) && (name == materialCache.getCachedStringVec()[selectedMaterial]));
	bool deleted = materialCache.RemoveID(name);
	if (deleted and selectedName) {
		selectedMaterial = -1;
		nameInput[0] = '\0';
		selectedTexture = -1;
		selectedHeightMapTexture = -1;
	}
	return deleted;
}

void MaterialManager::deleteAllMaterials() {
	selectedMaterial = -1;
	nameInput[0] = '\0';
	selectedTexture = -1;
	materialCache.clear();
}

void MaterialManager::imGuiRender() {

	const std::vector<const char*> CacheVec = materialCache.getCharVec();
	if (ImGui::Combo("Select Material", &selectedMaterial, CacheVec.data(), CacheVec.size())) {
		updateSelectedMaterial();
	}

	if (selectedMaterial >= 0) {
		//Get selected materials ID
		const std::string& matName = materialCache.getCachedName(selectedMaterial);
		//Get material from ID
		if (Material* mat = materialCache.tryGetValue(matName)) {
			//Output materials Name
			if (ImGui::InputText("Material Name", nameInput, sizeof(nameInput),
				ImGuiInputTextFlags_EnterReturnsTrue)) {

				// This block executes when the user presses Enter
				std::string newName(nameInput);
				if (materialCache.changeID(matName, newName)) {
					mat->MaterialName = newName;
					return;
				}
				else {
					updateSelectedMaterial();
				}
			}
			if (ImGui::TreeNode("Material: Colour")) {
				ImGui::ColorPicker4("Material Albedo: ", &mat->baseColour.x);
				ImGui::TreePop();
			}
			if (ImGui::TreeNode("Material: Specular")) {
				ImGui::ColorPicker4("Material Specular Colour: ", &mat->specularColour.x);
				ImGui::SliderFloat("Specular Power: ## 0", &mat->specularPower, 1.f, 1000.f);
				ImGui::TreePop();
			}
			bool isHeightMapShader = shaderManager->isHeightMapShader(mat->shaderName);
			if (shaderManager->selectGeometryShaderImGui(mat)) {
				if (!isHeightMapShader && shaderManager->isHeightMapShader(mat->shaderName)) {
					mat->HeightMapData = std::make_unique<HeightMapInfo>();
					mat->HeightMapData->HeightTextureString = L"";
					mat->HeightMapData->HeightMultiplier = 10.f;
				}
			}
			const std::vector<const char*>* imageList = FileHandler::get().getImageList();
			if (ImGui::Combo("Select Material Texture: ", &selectedTexture, imageList->data(), imageList->size())) {

				if (selectedTexture >= 0) {
					mat->textureString = textureManager->getTextureNameFromIndex(selectedTexture);
				}

				if (materialCache.hasInstances(matName)) {
					mat->texture = textureManager->getTexture(mat->textureString);
				}
			}

			if (mat->HeightMapData != nullptr) {
				const std::vector<const char*>* imageList = FileHandler::get().getImageList();
				if (ImGui::Combo("Select HeightMap Texture: ", &selectedHeightMapTexture, imageList->data(), imageList->size())) {
					if (selectedHeightMapTexture >= 0) {
						mat->HeightMapData->HeightTextureString = textureManager->getTextureNameFromIndex(selectedHeightMapTexture);
						if (materialCache.hasInstances(matName)) {
							mat->HeightMapData->HeightTexture = textureManager->getTexture(mat->HeightMapData->HeightTextureString);
						}
					}
					else {
						mat->HeightMapData->HeightTextureString = L"";
						mat->HeightMapData->HeightTexture = TextureInstance();
					}
				}

			}
		}
		
		if (ImGui::Button("Delete Material")) {
			deleteMaterial(matName);
		}
	}
}

void MaterialManager::to_json(nlohmann::json& j) {
	j = nlohmann::json::array();
	materialCache.forEach([&](const std::string& ID, Material* mat) {
		nlohmann::json matJson;
		matJson["Name"] = mat->MaterialName;
		matJson["Shader"] = mat->shaderName;
		if (!mat->textureString.empty()) {
			matJson["Texture"] = Converters::convert_from_wstring(mat->textureString);
		}
		matJson["BaseColour"] = mat->baseColour;
		matJson["SpecularColour"] = mat->specularColour;
		matJson["SpecularPower"] = mat->specularPower;
		if (mat->HeightMapData) {
			nlohmann::json& heightData = matJson["HeightMapData"];
			heightData["Multiplier"] = mat->HeightMapData->HeightMultiplier;
			heightData["Texture"] = Converters::convert_from_wstring(mat->HeightMapData->HeightTextureString);
		}
		j.push_back(matJson);
		});
}
void MaterialManager::from_json(const nlohmann::json& j) {
	deleteAllMaterials();
	for (auto matJson : j) {
		Material recreatedMat;

		recreatedMat.MaterialName = matJson.at("Name").get<std::string>();
		recreatedMat.shaderName = matJson.at("Shader").get<std::string>();
		recreatedMat.textureString = matJson.contains("Texture")
			? Converters::convert_to_wstring(matJson.at("Texture").get<std::string>())
			: L"";
		recreatedMat.baseColour = matJson.at("BaseColour").get<XMFLOAT4>();
		recreatedMat.specularColour = matJson.at("SpecularColour").get<XMFLOAT4>();
		recreatedMat.specularPower = matJson.at("SpecularPower").get<float>();
		if (matJson.contains("HeightMapData")) {
			const nlohmann::json& heightMapJson = matJson["HeightMapData"];
			recreatedMat.HeightMapData = std::make_unique<HeightMapInfo>();
			recreatedMat.HeightMapData->HeightTextureString = Converters::convert_to_wstring(heightMapJson.at("Texture").get<std::string>());
			recreatedMat.HeightMapData->HeightMultiplier = heightMapJson.at("Multiplier").get<float>();
		}
		materialCache.emplaceID(recreatedMat.MaterialName, std::move(recreatedMat));
	}
}