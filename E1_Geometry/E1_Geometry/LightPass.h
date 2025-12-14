#pragma once
#include "ToTexturePass.h"
#include "LightsDataModule.h"
#include "CameraDataModule.h"
#include "SamplerDataModule.h"
#include "TextureDataModule.h"
#include "MatrixDataModule.h"
#include "MaterialDataModule.h"

class LightPass : public ToTexturePass {
public:
	LightPass(
		size_t stage, passDependancies& deps, ShaderManager* shaderManager,
		InstanceManager* instanceManager, ID3D11Device* device, D3D* renderer,
		const RenderTextureData& RTData)
		: ToTexturePass(stage, deps, shaderManager, device, RTData),
		instanceManager(instanceManager), shaderManager(shaderManager), renderer(renderer)
	{
		
		texelSize = XMFLOAT2(1.f / static_cast<float>(RTData.screenWidth), 1.f / static_cast<float>(RTData.screenHeight));
		matrixDataModule = shaderManager->getShaderModuleID("MatrixDataModule");
		lightDataModule = shaderManager->getShaderModuleID("LightsDataModule");
		materialDataModule = shaderManager->getShaderModuleID("MaterialDataModule");
		cameraDataModule = shaderManager->getShaderModuleID("CameraDataModule");

		diffuseTextureDataModule = shaderManager->getShaderModuleID("TextureDataModule1");
		emissiveTextureDataModule = shaderManager->getShaderModuleID("TextureDataModule2");
		heightMapTextureModule = shaderManager->getShaderModuleID("TextureDataModule3");
		heightMapDataModule = shaderManager->getShaderModuleID("HeightMapDataModule");

		heightMapShader = shaderManager->getGeometryShader("HeightMapLightShader");

		shader = shaderManager->getGeometryShader("BasicLightShader");
	}

	void Render(ID3D11DeviceContext* deviceContext, ID3D11Device* device) {

		bool lightNumChanged = shaderManager->isModuleDirtyflagSet(DirtyModuleFlags::LIGHTNUMCHANGED);
		bool lightDataChanged = shaderManager->isModuleDirtyflagSet(DirtyModuleFlags::LIGHTSDATACHANGED);
		bool CameraChanged = shaderManager->isModuleDirtyflagSet(DirtyModuleFlags::CAMERA);

		// Clear the scene. (default blue colour)
		renderer->beginScene(1.0f, 1.0f, 1.0f, 1.0f);
		//Return if no lights
		textureOutput->setRenderTarget(renderer->getDeviceContext());
		textureOutput->clearRenderTarget(renderer->getDeviceContext(), 0.39f, 0.58f, 0.92f, 1.0f);
		//Check if camera changed
		if (CameraChanged || lightNumChanged) {
			//Update camera Data module
			shared_ptr<CameraDataModule> cameraData = std::dynamic_pointer_cast<CameraDataModule>(cameraDataModule->module);
			cameraData->setModuleParamaters(deviceContext, texelSize);
		}
		
		//Check if light number or data changed
		if (lightNumChanged || lightDataChanged) {

			// Update light data module
			shared_ptr<LightsDataModule> lightsData = std::dynamic_pointer_cast<LightsDataModule>(lightDataModule->module);
			lightsData->setModuleParamaters(deviceContext, nullptr);
		}

		size_t lightNum = instanceManager->getNumberOfLights();
		if (instanceManager->getNumberOfLights() == 0) { return; }
		std::shared_ptr<MatrixDataModule> matrixData = std::dynamic_pointer_cast<MatrixDataModule>(matrixDataModule->module);
		std::shared_ptr<MaterialDataModule> materialData = std::dynamic_pointer_cast<MaterialDataModule>(materialDataModule->module);
		
		XMMATRIX viewMatrix = instanceManager->getActiveCamera()->camera->getViewMatrix();
		const XMMATRIX& projectionMatrix = renderer->getProjectionMatrix();
		//Loop over geometry
		instanceManager->forEachMesh([&](size_t ID, GeometryData* meshInstance) {
			MeshType type = meshInstance->mesh->type;
			if (type != MeshType::ORTHO && type != MeshType::POINT)
			{
				Material* mat = meshInstance->mat;
				// Set matrix Data
				matrixData->setModuleParamaters(deviceContext, meshInstance, projectionMatrix, viewMatrix);
				// Set material Data
				materialData->setModuleParamaters(deviceContext, mat);
				// Set texture Data

				if (mat->diffuseTexture.IsValid()) {
					std::shared_ptr<TextureDataModule> diffuseModule = std::dynamic_pointer_cast<TextureDataModule>(diffuseTextureDataModule->module);
					diffuseModule->setModuleParamaters(deviceContext, mat->diffuseTexture->texture.Get());
				}

				if (mat->emissionTexture.IsValid()) {
					std::shared_ptr<TextureDataModule> emissiveModule = std::dynamic_pointer_cast<TextureDataModule>(emissiveTextureDataModule->module);
					emissiveModule->setModuleParamaters(deviceContext, mat->emissionTexture->texture.Get());
				}

				//Send Geometry Data
				std::shared_ptr<BaseMesh> mesh = meshInstance->mesh->mesh;

				if (mat->HeightMapData) {
					std::shared_ptr<TextureDataModule> heightMapData = std::dynamic_pointer_cast<TextureDataModule>(heightMapTextureModule->module);
					std::shared_ptr<HeightMapDataModule> heightData = std::dynamic_pointer_cast<HeightMapDataModule>(heightMapDataModule->module);

					heightMapData->setModuleParamaters(deviceContext, mat->HeightMapData->HeightTexture->texture.Get());
					heightData->setModuleParamaters(deviceContext, mat, meshInstance);

					mesh->sendData(deviceContext);
					heightMapShader->shader->setResources(deviceContext);
					heightMapShader->shader->render(deviceContext, mesh->getIndexCount());
				}
				else {
					mesh->sendData(deviceContext);
					shader->shader->setResources(deviceContext);
					shader->shader->render(deviceContext, mesh->getIndexCount());
				}
			}
		});
	};

	virtual void toJson(nlohmann::json& json) {}
	virtual void fromJson(const nlohmann::json& json) {}
private:
	D3D* renderer;
	InstanceManager* instanceManager;
	ShaderManager* shaderManager;

	XMFLOAT2 texelSize;

	//Vertex Shader Modules
	ModuleInstance matrixDataModule;	// Shaders Matrix Data
	ModuleInstance cameraDataModule;	// Camera Data Module

	//Pixel Shader Modules
	ModuleInstance lightDataModule;		//Lights data module
	ModuleInstance materialDataModule;	//Material Data module

	ModuleInstance diffuseTextureDataModule;	// Diffuse texture data module
	ModuleInstance emissiveTextureDataModule;	// Emissive texture data module

	ModuleInstance heightMapDataModule;
	ModuleInstance heightMapTextureModule;

	ShaderInstance heightMapShader;
};
