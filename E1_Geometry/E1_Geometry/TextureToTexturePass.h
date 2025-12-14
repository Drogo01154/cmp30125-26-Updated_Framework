#pragma once

#include "ToTexturePass.h"

class TextureToTexturePass : public ToTexturePass {
public:
	TextureToTexturePass(size_t stage,
		passDependancies& deps,
		InstanceManager* instanceManager,
		ShaderManager* shaderManager,
		D3D* renderer,
		const RenderTextureData& RTData,
		const OrthoMeshData& orthoData,
		const std::string& shaderName)
		: ToTexturePass(stage, deps, shaderManager, renderer->getDevice(), RTData),
		instanceManager(instanceManager),
		renderer(renderer)
	{
		this->shader = shaderManager->getPassShader(shaderName);
		textureDataModule = shaderManager->getShaderModuleID("TextureDataModule1");
		matrixDataModule = shaderManager->getShaderModuleID("MatrixDataModule");

		orthoMesh = instanceManager->createOrthoMeshInstance(orthoID, orthoData);

		matrixModule = dynamic_pointer_cast<MatrixDataModule>(matrixDataModule->module);
		textureModule = dynamic_pointer_cast<TextureDataModule>(textureDataModule->module);

		//Get previous pass
		std::shared_ptr<ToTexturePass> previousPass
			= dynamic_pointer_cast<ToTexturePass>(deps.begin()->second);
		inputTexture = previousPass->getTextureOutput();

		if (inputTexture == nullptr) { throw std::runtime_error("Error: no input texture!"); }
		updateModules = true;
	}

	virtual void Render(ID3D11DeviceContext* deviceContext, ID3D11Device* device) 
	{
		
		//Set render target back to back buffer

		//Return if no lights
		textureOutput->setRenderTarget(deviceContext);
		textureOutput->clearRenderTarget(deviceContext, 0.0f, 0.0f, 0.0f, 1.0f);

		XMMATRIX viewMatrix = instanceManager->getActiveCamera()->camera->getOrthoViewMatrix();
		matrixModule->setModuleParamaters(deviceContext, orthoMesh, renderer->getOrthoMatrix(), viewMatrix);
		textureModule->setModuleParamaters(deviceContext, inputTexture->getShaderResourceView());

		renderer->setZBuffer(false);
		orthoMesh->mesh->mesh->sendData(deviceContext);
		shader->shader->setResources(deviceContext);
		shader->shader->render(deviceContext, orthoMesh->mesh->mesh->getIndexCount());
		renderer->setZBuffer(true);
	}

	virtual void toJson(nlohmann::json& json) = 0;
	virtual void fromJson(const nlohmann::json& json) = 0;
protected:
	//Modules
	ModuleInstance textureDataModule;
	ModuleInstance matrixDataModule;
	std::shared_ptr<MatrixDataModule> matrixModule;
	std::shared_ptr<TextureDataModule> textureModule;

	InstanceManager* instanceManager;
	std::shared_ptr<RenderTexture> inputTexture;

	//Ortho Mesh
	size_t orthoID;
	GeometryInstance orthoMesh;

	D3D* renderer;
};
