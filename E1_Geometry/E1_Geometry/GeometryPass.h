#pragma once
#include "RenderToTexturePass.h"
#include "MatrixDataModule.h"

class GeometryPass : public RenderToTexturePass {
public:
	GeometryPass(size_t stage,
		passDependancies& deps,
		InstanceManager* instanceManager,
		ShaderManager* shaderManager,
		D3D* renderer,
		const RenderTextureData& RTData)
		: RenderToTexturePass(stage, deps, shaderManager, renderer->getDevice(), RTData),
		instanceManager(instanceManager),
		shaderManager(shaderManager),
		renderer(renderer)
	{
		matrixDataModule = shaderManager->getShaderModuleID("MatrixDataModule");
		colourShader = shaderManager->getGeometryShader("ColourShader");
	}

	void Render(ID3D11DeviceContext* deviceContext, ID3D11Device* device) {
		textureOutput->setRenderTarget(renderer->getDeviceContext());
		textureOutput->clearRenderTarget(renderer->getDeviceContext(), 0.39f, 0.58f, 0.92f, 1.0f);
		XMMATRIX viewMatrix = instanceManager->getActiveCamera()->camera->getViewMatrix();
		std::shared_ptr<MatrixDataModule> matrixModule = dynamic_pointer_cast<MatrixDataModule>(matrixDataModule->module);
		instanceManager->forEachMesh([&](const size_t& ID, GeometryData* instance) {
			if (instance->mat.IsValid()) {
				MeshType type = instance->mesh->type;
				if (type != MeshType::ORTHO || type != MeshType::POINT)
				{
					matrixModule->setModuleParamaters(deviceContext, instance, renderer->getProjectionMatrix(), viewMatrix);
					BaseMesh* mesh = instance->mesh->mesh.get();
					mesh->sendData(deviceContext);
					colourShader->shader->setResources(deviceContext);
					colourShader->shader->render(deviceContext, mesh->getIndexCount());
				}
			}
		});
	};
private:
	D3D* renderer;
	ShaderManager* shaderManager;
	InstanceManager* instanceManager;
	ModuleInstance matrixDataModule;	//Matrix Data Module;
	ShaderInstance colourShader;
};