#pragma once
#include "RenderPass.h"
#include "MatrixDataModule.h"

class GeometryPass : renderPass {
public:
	GeometryPass(InstanceManager* instanceManager, ShaderManager* shaderManager) : 
		instanceManager(instanceManager) ,
		shaderManager(shaderManager)
	{
		matrixDataModule = shaderManager->getShaderModuleID("MatrixDataModule");
	}

	void Render(ID3D11DeviceContext* deviceContext, ID3D11Device* device) {
		std::shared_ptr<MatrixDataModule> matrixModule = dynamic_pointer_cast<MatrixDataModule>(matrixDataModule->module);
		instanceManager->forEachMesh([&](const size_t& ID, GeometryData* instance) {
			std::shared_ptr<BaseShader> shader = instance->mat->shader->shader;
			const std::string& shaderType = instance->mat->shader->name;

			matrixModule->setModuleParamaters(deviceContext, instance, )
			BaseMesh* mesh = instance->mesh->mesh.get();


			shader->render(deviceContext, mesh->getIndexCount());
			});
	};
private:
	ShaderManager* shaderManager;
	InstanceManager* instanceManager;
	ModuleInstance matrixDataModule;	//Matrix Data Module;
};

/*
* 
* 