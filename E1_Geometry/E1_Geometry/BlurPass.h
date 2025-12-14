#pragma once
#include "TextureToTexturePass.h"
#include "ConstantBufferModule.h"


struct BlurBufferType
{
	bool horizontal;
	XMFLOAT2 texelSize;
	float padding;
};

#define BlurModule ConstantDataModule<BlurBufferType>

class BlurPass : public TextureToTexturePass {
public:
	BlurPass(size_t stage,
		passDependancies& deps,
		InstanceManager* instanceManager,
		ShaderManager* shaderManager,
		D3D* renderer,
		const RenderTextureData& RTData,
		const OrthoMeshData& orthoData)
		: TextureToTexturePass(
			stage, deps, instanceManager,
			shaderManager, renderer, RTData, orthoData, "GausseanShader")
	{

		blurBuffer.texelSize = { 1.f / RTData.screenWidth, 1.f / RTData.screenHeight };

		blurModuleInstance = shaderManager->getShaderModuleID("BlurModule");
		blurModule = std::dynamic_pointer_cast<BlurModule>(blurModuleInstance->module);

		secondTextureOutput = std::make_shared<RenderTexture>(renderer->getDevice(), RTData);
	}

	void Render(ID3D11DeviceContext* deviceContext, ID3D11Device* device) {
		bool horizontal = true, first_iteration = true;
		int amount = 10;

		XMMATRIX viewMatrix = instanceManager->getActiveCamera()->camera->getOrthoViewMatrix();
		matrixModule->setModuleParamaters(deviceContext, orthoMesh, renderer->getOrthoMatrix(), viewMatrix);
		
		std::shared_ptr<RenderTexture> ping = textureOutput;
		std::shared_ptr<RenderTexture> pong = secondTextureOutput;

		for (unsigned int i = 0; i < amount; i++)
		{
			if (first_iteration) {
				textureModule->setModuleParamaters(deviceContext, inputTexture->getShaderResourceView());
				first_iteration = false;
			}
			else {
				textureModule->setModuleParamaters(deviceContext, ping->getShaderResourceView());
			}

			pong->setRenderTarget(renderer->getDeviceContext());
			pong->clearRenderTarget(renderer->getDeviceContext(), 0.0f, 0.0f, 0.0f, 1.0f);

			blurBuffer.horizontal = horizontal;
			blurModule->setModuleParamaters(deviceContext, &blurBuffer);
			
			renderer->setZBuffer(false);
			orthoMesh->mesh->mesh->sendData(deviceContext);
			shader->shader->setResources(deviceContext);
			shader->shader->render(deviceContext, orthoMesh->mesh->mesh->getIndexCount());
			renderer->setZBuffer(true);

			std::swap(ping, pong);

			horizontal = !horizontal;
		}
	}

	virtual void toJson(nlohmann::json& json) {}
	virtual void fromJson(const nlohmann::json& json) {}
private:
	//Modules
	ModuleInstance blurModuleInstance;
	std::shared_ptr<BlurModule> blurModule;
	BlurBufferType blurBuffer;
	std::shared_ptr<RenderTexture> secondTextureOutput;
};