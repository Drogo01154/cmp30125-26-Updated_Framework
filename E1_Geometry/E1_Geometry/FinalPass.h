#pragma once
#include "RenderToTexturePass.h"
#include "TextureDataModule.h"

class FinalRenderPass : public renderPass {
public:
	FinalRenderPass(size_t stage, const passDependancies& deps, D3D* renderer, ShaderManager* shaderManager, InstanceManager* instanceManager, const OrthoMeshData& orthoData) : renderPass(stage, deps), renderer(renderer), instanceManager(instanceManager) {
		textureDataModule = shaderManager->getShaderModuleID("TextureDataModule");
		matrixDataModule = shaderManager->getShaderModuleID("MatrixDataModule");

		shader = shaderManager->getPassShader("TextureShader");

		orthoMesh = instanceManager->createOrthoMeshInstance(orthoID, orthoData);
		inputTexture = std::dynamic_pointer_cast<RenderToTexturePass>(deps.begin()->second)->getTextureOutput();
		if (inputTexture == nullptr) { throw std::runtime_error("Error: no input texture!"); }
	}

	void Render(ID3D11DeviceContext* deviceContext, ID3D11Device* device) {
		XMMATRIX viewMatrix = instanceManager->getActiveCamera()->camera->getOrthoViewMatrix();
		//Set render target back to back buffer
		renderer->setBackBufferRenderTarget();
		renderer->resetViewport();

		std::shared_ptr<MatrixDataModule> matrixModule = dynamic_pointer_cast<MatrixDataModule>(matrixDataModule->module);
		std::shared_ptr<TextureDataModule> textureModule = dynamic_pointer_cast<TextureDataModule>(textureDataModule->module);
		matrixModule->setModuleParamaters(deviceContext, orthoMesh, renderer->getOrthoMatrix(), viewMatrix);
		textureModule->setModuleParamaters(deviceContext, inputTexture->getShaderResourceView());

		renderer->setZBuffer(false);
		orthoMesh->mesh->mesh->sendData(deviceContext);
		shader->shader->setResources(deviceContext);
		shader->shader->render(deviceContext, orthoMesh->mesh->mesh->getIndexCount());
		renderer->setZBuffer(true);
	}

private:
	InstanceManager* instanceManager;
	//Manager Ptrs
	ShaderInstance shader;

	//Modules
	ModuleInstance textureDataModule;
	ModuleInstance matrixDataModule;

	//Ortho Mesh
	size_t orthoID;
	GeometryInstance orthoMesh;

	D3D* renderer;

	std::shared_ptr<RenderTexture> inputTexture; // Pointer to input texture from other render pass;
};