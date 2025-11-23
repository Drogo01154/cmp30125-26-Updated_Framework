#pragma once
#include "RenderPass.h"
#include "DXF.h"
#include "CameraDataModule.h"
#include "MatrixDataModule.h"
#include "TextureDataModule.h"



class RenderTexturePass : renderPass {
public:
	RenderTexturePass(ShaderManager* shaderManager, 
		InstanceManager* instanceManager, 
		ID3D11Device* device, 
		const RenderTextureData& mainRTData,
		const OrthoMeshData& mainOrthoData)
		: renderPass(),
		instanceManager(instanceManager),
		shaderManager(shaderManager)
	{
		updateModules = true;
		textureOutput = std::make_shared<RenderTexture>(device, mainRTData);
		
		textureDataModule = shaderManager->getShaderModuleID("TextureDataModule");
		cameraDataModule = shaderManager->getShaderModuleID("ShaderDataModule");
		matrixDataModule = shaderManager->getShaderModuleID("MatrixDataModule");
		orthoMesh = instanceManager->createOrthoMeshInstance(orthoID, mainOrthoData);
	}

	std::shared_ptr<RenderTexture> getTextureOutput() { return textureOutput; }
	void setTextureInput(std::shared_ptr<RenderTexture> textureInput) { this->textureInput = textureInput; }

	virtual void setPassParamaters() {
	
	}

	void Render(ID3D11DeviceContext* device, const XMMATRIX& projectionMatrix) {
		
		orthoMesh->mesh->mesh->sendData(device);
		if (updateModules) {
			std::shared_ptr<MatrixDataModule> matrixModule = dynamic_pointer_cast<MatrixDataModule>(matrixDataModule->module);
			std::shared_ptr<CameraDataModule> camModule = dynamic_pointer_cast<CameraDataModule>(cameraDataModule->module);
			std::shared_ptr<TextureDataModule> textureModule = dynamic_pointer_cast<TextureDataModule>(textureDataModule->module);
			matrixModule->setModuleParamaters(device, orthoMesh, projectionMatrix);
			camModule->setModuleParamaters(device);
			textureModule->setModuleParamaters(device, textureOutput->getShaderResourceView());
		}
		shader->shader->setResources(device);
		shader->shader->render(device, orthoMesh->mesh->mesh->getIndexCount());
	}

	void setUpdateModules(bool value) { updateModules = value; }
protected:
	//Manager Ptrs
	ShaderManager* shaderManager;
	InstanceManager* instanceManager;

	//Modules
	ModuleInstance textureDataModule;
	ModuleInstance cameraDataModule;
	ModuleInstance matrixDataModule;
	ShaderInstance shader;

	//Ortho Mesh
	size_t orthoID;
	GeometryInstance orthoMesh;

	//Input and output render textures
	std::shared_ptr<RenderTexture> textureInput; // Pointer to input texture from other render pass;
	std::shared_ptr<RenderTexture> textureOutput; // Pointer to render pass output

	bool updateModules;
};