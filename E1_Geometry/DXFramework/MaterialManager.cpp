#include "MaterialManager.h"

//Constructor
MaterialManager::MaterialManager(ShaderManager* shaderManager, TextureManager* textureManager) :
	materialCache(10, false),
	shaderManager(shaderManager),
	textureManager(textureManager) {
	materialCache.addTypeInitialiser([&](Material* mat) {
		if (!mat->diffuseTextureString.empty()) {
			mat->diffuseTexture = this->textureManager->getTexture(mat->diffuseTextureString);
		}

		if (!mat->normalTextureString.empty()) {
			mat->normalTexture = this->textureManager->getTexture(mat->normalTextureString);
		}

		if (!mat->emissiveTextureString.empty()) {
			mat->emissionTexture = this->textureManager->getTexture(mat->emissiveTextureString);
		}

		if (mat->HeightMapData != nullptr && !mat->HeightMapData->HeightTextureString.empty()) {
			mat->HeightMapData->HeightTexture = this->textureManager->getTexture(mat->HeightMapData->HeightTextureString);
		}
	});

	materialCache.addTypeEndHandler([](Material* mat) {
		mat->HeightMapData.reset();
	});


	selectedMaterial = -1;
	selectedDiffuseTexture = -1;
	selectedEmissiveTexture = -1;
	selectedNormalTexture = -1;
	selectedHeightMapTexture = -1;

	createMaterial("Default");
	//Material& mat = materialCache.getValue("Default");
	//mat.HeightMapData = std::make_unique<HeightMapInfo>();
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
	mat.diffuseTextureString = L"DefaultDiffuse";
	mat.normalTextureString = L"";
	mat.emissiveTextureString = L"";
	mat.emissiveStrength = 1.f;
	mat.HeightMapData = nullptr;

	materialCache.emplaceID(name, std::move(mat), false);
}

void MaterialManager::updateSelectedMaterial() {

	// Reset state by default
	existingNameInput[0] = '\0';
	selectedDiffuseTexture = -1;
	selectedEmissiveTexture = -1;
	selectedNormalTexture = -1;
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
	strncpy_s(existingNameInput, mat->MaterialName.c_str(), sizeof(existingNameInput));

	const auto& imageList = *FileHandler::get().getImageList();

	auto getSelectedTexture = [&](int& selectedValue, const std::wstring& textureWString) {
		if (!textureWString.empty()) {
			std::string textureString = Converters::convert_from_wstring(textureWString);

			// Find matching texture
			for (int i = 0; i < imageList.size(); i++) {
				if (textureString == std::string(imageList[i])) {
					selectedValue = i;
					break;
				}
			}
		}
	};

	getSelectedTexture(selectedDiffuseTexture, mat->diffuseTextureString);
	getSelectedTexture(selectedEmissiveTexture, mat->emissiveTextureString);
	getSelectedTexture(selectedNormalTexture, mat->normalTextureString);
	if (mat->HeightMapData != nullptr) {
		getSelectedTexture(selectedHeightMapTexture, mat->HeightMapData->HeightTextureString);
	}
}

bool MaterialManager::deleteMaterial(const std::string& name) {
	bool selectedName = ((selectedMaterial >= 0) && (name == materialCache.getCachedStringVec()[selectedMaterial]));
	bool deleted = materialCache.RemoveID(name);
	if (deleted and selectedName) {
		selectedMaterial = -1;
		existingNameInput[0] = '\0';
		selectedMaterial = -1;
		selectedDiffuseTexture = -1;
		selectedEmissiveTexture = -1;
		selectedNormalTexture = -1;
		selectedHeightMapTexture = -1;
	}
	return deleted;
}

void MaterialManager::deleteAllMaterials() {
	selectedMaterial = -1;
	newNameInput[0] = '\0';
	existingNameInput[0] = '\0';
	selectedMaterial = -1;
	selectedDiffuseTexture = -1;
	selectedEmissiveTexture = -1;
	selectedNormalTexture = -1;
	selectedHeightMapTexture = -1;
	materialCache.clear();
}

const std::vector<std::string>* MaterialManager::getMaterialStrings() {
	return &materialCache.getCachedStringVec();
}
const std::vector<const char*>* MaterialManager::getMatrialsChars() {
	return &materialCache.getCharVec();
}

void MaterialManager::imGuiRender() {

	if (ImGui::CollapsingHeader("Material Settings: ")) {

		ImGui::InputText("New Material Name", newNameInput, sizeof(newNameInput));
		if (ImGui::Button("Create Material")) {
			std::string newMaterialName(newNameInput);
			if (!newMaterialName.empty()) {
				createMaterial(newMaterialName);
				selectedMaterial = materialCache.size() - 1;
				updateSelectedMaterial();
			}
		}
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
				if (ImGui::InputText("Material Name", existingNameInput, sizeof(existingNameInput),
					ImGuiInputTextFlags_EnterReturnsTrue)) {

					// This block executes when the user presses Enter
					std::string newName(existingNameInput);
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
				if (mat->HeightMapData == nullptr && ImGui::Button("Make Heightmap")) {
					mat->HeightMapData = std::make_unique<HeightMapInfo>();
					HeightMapInfo* mapInfo = mat->HeightMapData.get();
					if (materialCache.hasInstances(matName)) {
						mapInfo->HeightTexture = textureManager->getTexture(mapInfo->HeightTextureString);
					}

					const auto& imageList = *FileHandler::get().getImageList();
					std::string textureName = Converters::convert_from_wstring(mapInfo->HeightTextureString);

					// Find matching texture
					for (int i = 0; i < imageList.size(); i++) {
						if (textureName == imageList[i]) {
							selectedHeightMapTexture = i;
							break;
						}
					}
				}

				if (!mat->emissiveTextureString.empty()) {
					ImGui::SliderFloat("Emissive Strength: ", &mat->emissiveStrength, 0.1f, 10.f);
				}

				const std::vector<const char*>* imageList = FileHandler::get().getImageList();

				auto ComboImageSelectFunction = [&](const char* ImGuiTag, int& selectedValue, std::wstring* textureString, TextureInstance* textureInstance)
					{
						if (ImGui::Combo(ImGuiTag, &selectedValue, imageList->data(), imageList->size())) {
							if (selectedValue >= 0) {
								std::string inputString = std::string((*imageList)[selectedValue]);
								std::wstring inputWString = Converters::convert_to_wstring(inputString);
								*textureString = inputWString;

								*textureInstance = materialCache.hasInstances(matName)
									? textureManager->getTexture(*textureString)
									: TextureInstance();
							}
							else {
								*textureString = L"";
								*textureInstance = TextureInstance();
							}
							updateSelectedMaterial();
						}
					};
				ComboImageSelectFunction("Select Diffuse Texture: ", selectedDiffuseTexture, &mat->diffuseTextureString, &mat->diffuseTexture);
				ComboImageSelectFunction("Select Normal Texture: ", selectedNormalTexture, &mat->normalTextureString, &mat->normalTexture);
				ComboImageSelectFunction("Select Emission Texture: ", selectedEmissiveTexture, &mat->emissiveTextureString, &mat->emissionTexture);

				if (mat->HeightMapData != nullptr) {
					ImGui::SliderFloat("Height Map Multiplier: ", &mat->HeightMapData->HeightMultiplier, 0.1f, 1000.f);
					ImGui::SliderFloat("UV scale", &mat->HeightMapData->UVScale, 0.01f, 100.f);
					ComboImageSelectFunction("Select Height Map Texture: ", selectedHeightMapTexture, &mat->HeightMapData->HeightTextureString, &mat->HeightMapData->HeightTexture);
				}

				if (!mat->diffuseTextureString.empty() && ImGui::Button("Remove Diffuse Texture")) {
					mat->diffuseTextureString = L"";
					mat->diffuseTexture = TextureInstance();
					selectedDiffuseTexture = -1;
				}
				if (!mat->normalTextureString.empty() && ImGui::Button("Remove Normal Texture Map")) {
					mat->normalTextureString = L"";
					mat->normalTexture = TextureInstance();
					selectedNormalTexture = -1;
				}
				if (!mat->emissiveTextureString.empty() && ImGui::Button("Remove Emissive Texture")) {
					mat->emissiveTextureString = L"";
					mat->emissionTexture = TextureInstance();
					selectedEmissiveTexture = -1;
				}
			}

			if (ImGui::Button("Delete Material")) {
				deleteMaterial(matName);
			}
		}
	}
}

void MaterialManager::to_json(nlohmann::json& j) {
	j = nlohmann::json::array();
	materialCache.forEach([&](const std::string& ID, Material* mat) {
		nlohmann::json matJson;
		matJson["Name"] = mat->MaterialName;

		if (!mat->diffuseTextureString.empty()) {
			matJson["DiffuseTexture"] = Converters::convert_from_wstring(mat->diffuseTextureString);
		}
		if (!mat->normalTextureString.empty()) {
			matJson["NormalTexture"] = Converters::convert_from_wstring(mat->normalTextureString);
		}
		if (!mat->emissiveTextureString.empty()) {
			matJson["EmissiveTexture"] = Converters::convert_from_wstring(mat->emissiveTextureString);
		}
		matJson["EmissiveStrength"] = mat->emissiveStrength;
		matJson["BaseColour"] = mat->baseColour;
		matJson["SpecularColour"] = mat->specularColour;
		matJson["SpecularPower"] = mat->specularPower;
		if (mat->HeightMapData) {
			nlohmann::json& heightData = matJson["HeightMapData"];
			heightData["Multiplier"] = mat->HeightMapData->HeightMultiplier;
			heightData["Texture"] = Converters::convert_from_wstring(mat->HeightMapData->HeightTextureString);
			heightData["UVScale"] = mat->HeightMapData->UVScale;
		}
		j.push_back(matJson);
		});
}
void MaterialManager::from_json(const nlohmann::json& j) {
	deleteAllMaterials();
	for (auto matJson : j) {
		std::string matName = matJson.at("Name").get<std::string>();

		Material mat;

		mat.MaterialName = matName;
		mat.baseColour = matJson.at("BaseColour").get<XMFLOAT4>();
		mat.specularColour = matJson.at("SpecularColour").get<XMFLOAT4>();
		mat.specularPower = matJson.at("SpecularPower").get<float>();
		mat.emissiveStrength = matJson.at("EmissiveStrength").get<float>();
		
		mat.diffuseTextureString = matJson.contains("DiffuseTexture") ? Converters::convert_to_wstring(matJson.at("DiffuseTexture").get<std::string>()) : L"";
		mat.normalTextureString = matJson.contains("NormalTexture") ? Converters::convert_to_wstring(matJson.at("NormalTexture").get<std::string>()) : L"";
		mat.emissiveTextureString = matJson.contains("EmissiveTexture") ? Converters::convert_to_wstring(matJson.at("EmissiveTexture").get<std::string>()) : L"";

		if (matJson.contains("HeightMapData")) {
			const nlohmann::json& heightMapJson = matJson["HeightMapData"];
			mat.HeightMapData = std::make_unique<HeightMapInfo>();
			mat.HeightMapData->HeightTextureString = Converters::convert_to_wstring(heightMapJson.at("Texture").get<std::string>());
			mat.HeightMapData->HeightMultiplier = heightMapJson.at("Multiplier").get<float>();
			mat.HeightMapData->UVScale = heightMapJson.contains("UVScale") ? heightMapJson.at("UVScale").get<float>() : 1.f;
		}
		else {
			mat.HeightMapData = nullptr;
		}
		
		materialCache.emplaceID(matName, std::move(mat), false); 
	}
}