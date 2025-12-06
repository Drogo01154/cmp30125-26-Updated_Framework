#pragma once
#include "TextureToTexturePass.h"
#include "ConstantBufferModule.h"


struct BloomBufferType {
	float threshold;
	float exposure;
	XMFLOAT2 padding;
};

#define BloomModule ConstantDataModule<BloomBufferType>

class ThresholdPass : public TextureToTexturePass {
public:
	ThresholdPass(size_t stage,
		passDependancies& deps,
		InstanceManager* instanceManager,
		ShaderManager* shaderManager,
		D3D* renderer,
		const RenderTextureData& RTData,
		const OrthoMeshData& orthoData)
		: TextureToTexturePass(
			stage, deps, instanceManager, 
			shaderManager, renderer, RTData, orthoData, "BloomThresholdShader")
	{
		hasImGui = true;
		//Set bloom values
		bloomBuffer.threshold = 1.f;
		bloomBuffer.exposure = 1.f;

		bloomModuleInstance = shaderManager->getShaderModuleID("BloomDataModule");
		bloomModule = dynamic_pointer_cast<BloomModule>(bloomModuleInstance->module);
		updateModules = true;
	}

	void Render(ID3D11DeviceContext* deviceContext, ID3D11Device* device) {
		if (updateModules) {
			bloomModule->setModuleParamaters(deviceContext, &bloomBuffer);
		}
		TextureToTexturePass::Render(deviceContext, device);
	}

	void ImGuiMenu() override 
	{
		// Inside your ImGui frame:
		if (ImGui::CollapsingHeader("Bloom Settings")) {
			if (ImGui::SliderFloat("Threshold", &bloomBuffer.threshold, 0.0f, 5.0f, "%.2f")) { updateModules = true; }
			if (ImGui::SliderFloat("Eposure", &bloomBuffer.exposure, 0.0f, 100.0f, "%.2f")) { updateModules = true; }
		}
	}
private: 
	//Modules
	ModuleInstance bloomModuleInstance;
	std::shared_ptr<BloomModule> bloomModule;
	BloomBufferType bloomBuffer;
};