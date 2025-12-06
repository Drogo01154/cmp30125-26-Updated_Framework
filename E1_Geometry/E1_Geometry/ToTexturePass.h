#pragma once
#include "RenderPass.h"
#include "DXF.h"
#include "CameraDataModule.h"
#include "MatrixDataModule.h"
#include "TextureDataModule.h"
#include "RenderTexture.h"
#include <memory>

class ToTexturePass : public renderPass {
public:
	ToTexturePass(size_t stage,
		passDependancies& deps,
		ShaderManager* shaderManager,
		ID3D11Device* device, 
		const RenderTextureData& RTData)
		: renderPass(stage, deps)
	{
		updateModules = true;
		textureOutput = std::make_shared<RenderTexture>(device, RTData);
	}

	std::shared_ptr<RenderTexture> getTextureOutput() { return textureOutput; }

	void Render(ID3D11DeviceContext* deviceContext, ID3D11Device* device) = 0;

protected:

	//Modules
	ShaderInstance shader;

	//Input and output render textures
	
	std::shared_ptr<RenderTexture> textureOutput; // Pointer to render pass output

	bool updateModules;
};