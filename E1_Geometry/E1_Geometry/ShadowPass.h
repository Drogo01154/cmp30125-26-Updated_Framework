#pragma once
#include "DepthPass.h"
#include "RenderToTexturePass.h"
#include "LightsDataModule.h"
#include "CameraDataModule.h"
#include "PointLightDataModule.h"
#include "ShadowMapDataModule.h"
#include "SamplerDataModule.h"
#include "TextureDataModule.h"
#include "MatrixDataModule.h"
#include "MaterialDataModule.h"

class ShadowPass : public RenderToTexturePass {
public:
	ShadowPass(
		size_t stage, passDependancies& deps, ShaderManager* shaderManager,
		InstanceManager* instanceManager, ID3D11Device* device, D3D* renderer,
		const RenderTextureData& RTData) 
		: RenderToTexturePass(stage, deps, shaderManager, device, RTData),
		instanceManager(instanceManager), shaderManager(shaderManager), renderer(renderer)
	{
		matrixDataModule = shaderManager->getShaderModuleID("MatrixDataModule");
		lightDataModule = shaderManager->getShaderModuleID("LightsDataModule");
		materialDataModule = shaderManager->getShaderModuleID("MaterialDataModule");
		cameraDataModule = shaderManager->getShaderModuleID("CameraDataModule");
		shadowMapDataModule = shaderManager->getShaderModuleID("ShadowMapDataModule");
		textureDataModule = shaderManager->getShaderModuleID("TextureDataModule");
		shader = shaderManager->getGeometryShader("ShadowShader");

		depthPass = std::dynamic_pointer_cast<DepthPass>(deps.begin()->second);
	}

	void Render(ID3D11DeviceContext* deviceContext, ID3D11Device* device) {

		// Clear the scene. (default blue colour)
		renderer->beginScene(1.0f, 1.0f, 1.0f, 1.0f);
		//Return if no lights
		textureOutput->setRenderTarget(renderer->getDeviceContext());
		textureOutput->clearRenderTarget(renderer->getDeviceContext(), 0.39f, 0.58f, 0.92f, 1.0f);
		//Check if camera changed
		if (shaderManager->isModuleDirtyflagSet(DirtyModuleFlags::CAMERA)) {
			//Update camera Data module
			shared_ptr<CameraDataModule> cameraData = std::dynamic_pointer_cast<CameraDataModule>(cameraDataModule->module);
			cameraData->setModuleParamaters(deviceContext);
		}
		//Check if light number or data changed
		if (shaderManager->isModuleDirtyflagSet(DirtyModuleFlags::LIGHTNUMCHANGED) ||
			shaderManager->isModuleDirtyflagSet(DirtyModuleFlags::LIGHTSDATACHANGED)) {
			
			// Update light data module
			shared_ptr<LightsDataModule> lightsData = std::dynamic_pointer_cast<LightsDataModule>(lightDataModule->module);
			lightsData->setModuleParamaters(deviceContext, depthPass->getLightIndexes());
			// Calculate lightViewProj Matrices
			std::vector<XMMATRIX> lightViewProjMatrices;
			instanceManager->forEachLight([&](size_t ID, Light* light) {
				lightTypes type = light->getType();
				if (type != lightTypes::point) {
					lightViewProjMatrices.push_back(XMMatrixMultiply(light->getViewMatrix(0), light->getProjectionMatrix()));
				}
				});

			//Update shadow map module
			shared_ptr<ShadowMapDataModule> shadowMapData = std::dynamic_pointer_cast<ShadowMapDataModule>(shadowMapDataModule->module);
			ShadowMapArray* shadowMapArray = depthPass->getShadowMapArray();
			CubeMapArray* cubeMapArray = depthPass->getCubeMapArray();
			shadowMapData->setModuleParamaters(
				deviceContext, 
				&lightViewProjMatrices, shadowMapArray->getDepthMapArraySRV(),
				cubeMapArray->getCubeMapArraySRV());
		
		}
		size_t lightNum = instanceManager->getNumberOfLights();
		if (instanceManager->getNumberOfLights() == 0) { return; }
		std::shared_ptr<MatrixDataModule> matrixData = std::dynamic_pointer_cast<MatrixDataModule>(matrixDataModule->module);
		std::shared_ptr<MaterialDataModule> materialData = std::dynamic_pointer_cast<MaterialDataModule>(materialDataModule->module);
		std::shared_ptr<TextureDataModule> textureData = std::dynamic_pointer_cast<TextureDataModule>(textureDataModule->module);
		
		XMMATRIX viewMatrix = instanceManager->getActiveCamera()->camera->getViewMatrix();
		const XMMATRIX& projectionMatrix = renderer->getProjectionMatrix();
		//Loop over geometry
		instanceManager->forEachMesh([&](size_t ID, GeometryData* meshInstance) {
			MeshType type = meshInstance->mesh->type;
			if(type != MeshType::ORTHO && type != MeshType::POINT) 
			{
				Material* mat = meshInstance->mat;
				// Set matrix Data
				matrixData->setModuleParamaters(deviceContext, meshInstance, projectionMatrix, viewMatrix);
				// Set material Data
				materialData->setModuleParamaters(deviceContext, mat);
				// Set texture Data
				textureData->setModuleParamaters(deviceContext, mat->texture->texture.Get());

				//Send Geometry Data
				std::shared_ptr<BaseMesh> mesh = meshInstance->mesh->mesh;
				mesh->sendData(deviceContext);
				shader->shader->setResources(deviceContext);
				shader->shader->render(deviceContext, mesh->getIndexCount());
			}
			});
	};
private:

	D3D* renderer;
	InstanceManager* instanceManager;
	ShaderManager* shaderManager;
	std::shared_ptr<DepthPass> depthPass;

	//Vertex Shader Modules
	ModuleInstance matrixDataModule;	// Shaders Matrix Data
	ModuleInstance cameraDataModule;	// Camera Data Module

	//Pixel Shader Modules
	ModuleInstance lightDataModule;		//Lights data module
	ModuleInstance materialDataModule;	//Material Data module
	ModuleInstance shadowMapDataModule;	//Module for shadow maps

	ModuleInstance textureDataModule;	//Module for materials texture
};
