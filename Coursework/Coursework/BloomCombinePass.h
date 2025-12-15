#pragma once

#pragma once
#include "ThresholdPass.h"

/*
	Deps order 
	1. HDR Pass
	2. blurPass
*/
class BloomCombinePass : public TextureToTexturePass {
public:
	BloomCombinePass(size_t stage,
		passDependancies& deps,
		InstanceManager* instanceManager,
		ShaderManager* shaderManager,
		D3D* renderer,
		const RenderTextureData& RTData,
		const OrthoMeshData& orthoData)
		: TextureToTexturePass(
			stage, deps, instanceManager,
			shaderManager, renderer, RTData, orthoData, "BloomCombineShader")
	{

		bloomModuleInstance = shaderManager->getShaderModuleID("BloomDataModule");
		bloomModule = std::dynamic_pointer_cast<BloomModule>(bloomModuleInstance->module);

		blurTextureModuleInstance = shaderManager->getShaderModuleID("TextureDataModule2");
		blurTextureModule = std::dynamic_pointer_cast<TextureDataModule>(blurTextureModuleInstance->module);

		//Get previous pass
		auto it = deps.begin();
		++it;
		std::shared_ptr<ToTexturePass> blurPass =
			std::dynamic_pointer_cast<ToTexturePass>(it->second);
		blurPassTexture = blurPass->getTextureOutput();
	}

	void Render(ID3D11DeviceContext* deviceContext, ID3D11Device* device) {
		XMMATRIX viewMatrix = instanceManager->getActiveCamera()->camera->getOrthoViewMatrix();
		matrixModule->setModuleParamaters(deviceContext, orthoMesh, renderer->getOrthoMatrix(), viewMatrix);

		textureModule->setModuleParamaters(deviceContext, inputTexture->getShaderResourceView());
		blurTextureModule->setModuleParamaters(deviceContext, blurPassTexture->getShaderResourceView());

		textureOutput->setRenderTarget(deviceContext);
		textureOutput->clearRenderTarget(deviceContext, 0.0f, 0.0f, 0.0f, 1.0f);

		renderer->setZBuffer(false);
		orthoMesh->mesh->mesh->sendData(deviceContext);
		shader->shader->setResources(deviceContext);
		shader->shader->render(deviceContext, orthoMesh->mesh->mesh->getIndexCount());
		renderer->setZBuffer(true);
	}

	virtual void toJson(nlohmann::json& json) {}
	virtual void fromJson(const nlohmann::json& json) {}
private:
	ModuleInstance bloomModuleInstance;
	std::shared_ptr<BloomModule> bloomModule;

	ModuleInstance blurTextureModuleInstance;
	std::shared_ptr<TextureDataModule> blurTextureModule;

	std::shared_ptr<RenderTexture> blurPassTexture;
};