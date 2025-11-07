#include "MiniMapTextureShader.h"

MiniMapTextureShader::MiniMapTextureShader(ID3D11Device* device, HWND hwnd, int screenWidth, int screenHeight) : 
	BaseShader(device, hwnd), 
	matrixDataModule(device, hwnd)
{
	screenSize = { float(screenWidth), float(screenHeight) };
	initShader(L"texture_vs.cso", L"textureMiniMap_ps.cso");

	miniMapWidth = 20.f;
	miniMapHeight = 20.f;
	miniMapNearZ = 0.1f;
	miniMapFarZ = 100.f;

	playerIconColour = XMFLOAT4(1.f, 0.0f, 0.0f, 1.0f);
	playerIconRadius = 5.f;
	greyScaleValues = { 0.299, 0.587, 0.114 };

	//Calulcate the orthographic projection matrix
	orthographicProjectionMatrix = XMMatrixOrthographicLH(miniMapWidth, miniMapHeight, miniMapNearZ, miniMapFarZ);
}

MiniMapTextureShader::~MiniMapTextureShader()
{
	// Release the layout.
	if (layout)
	{
		layout->Release();
		layout = 0;
	}

	//Release base shader components
	BaseShader::~BaseShader();
}


void MiniMapTextureShader::initShader(const wchar_t* vsFilename, const wchar_t* psFilename)
{
	D3D11_SAMPLER_DESC samplerDesc;
	D3D11_BUFFER_DESC miniMapBufferDesc;

	// Setup the description of the dynamic camera constant buffer that is used in the vertex shader
	miniMapBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	miniMapBufferDesc.ByteWidth = sizeof(miniMapDataType);
	miniMapBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	miniMapBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	miniMapBufferDesc.MiscFlags = 0;
	miniMapBufferDesc.StructureByteStride = 0;
	HRESULT result = renderer->CreateBuffer(&miniMapBufferDesc, NULL, miniMapBuffer.GetAddressOf());
	if (FAILED(result))
	{
		assert(false);
	}


	// Load (+ compile) shader files
	loadVertexShader(vsFilename);
	loadPixelShader(psFilename);

	matrixDataModule.initModule();


	// Create a texture sampler state description.
	samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	// Create the texture sampler state.
	renderer->CreateSamplerState(&samplerDesc, sampleState.GetAddressOf());

}


void MiniMapTextureShader::setShaderParameters(ID3D11DeviceContext* deviceContext,
	const XMMATRIX& worldMatrix,
	const XMMATRIX& viewMatrix,
	const XMMATRIX& projectionMatrix,
	ID3D11ShaderResourceView* texture,
	Camera* camera,
	Camera* miniMapCamera)
{
	XMFLOAT3 cameraPos = camera->getPosition();

	XMVECTOR projectedPos = XMVector3Project(
		XMLoadFloat3(&cameraPos),
		0.0f, 0.0f,
		screenSize.x,
		screenSize.y,
		0.0f,
		1.0f,
		orthographicProjectionMatrix,
		miniMapCamera->getViewMatrix(),
		XMMatrixIdentity()
	);

	XMFLOAT3 screenPos;
	XMStoreFloat3(&screenPos, projectedPos);


	//Calculate miniMap width and height
	float miniMapWidth = screenSize.x / 4.f;
	float miniMapHeight = screenSize.y / 4.f;

	//Calculate offset to recentre minimmap
	float xOffset = (screenSize.x - miniMapWidth) / 2.f;
	float yOffset = (screenSize.y - miniMapHeight) / 2.f;

	//Scale down target to minimap size and recentre
	screenPos.x = screenPos.x / 4.f + xOffset;
	screenPos.y = screenPos.y / 4.f + yOffset;

	// Offset to top right of screen
	screenPos.x += screenSize.x / 2.7f;
	screenPos.y -= screenSize.y / 2.7f;

	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	XMMATRIX tworld, tview, tproj;

	// Transpose the matrices to prepare them for the shader.
	tworld = XMMatrixTranspose(worldMatrix);
	tview = XMMatrixTranspose(viewMatrix);
	tproj = XMMatrixTranspose(projectionMatrix);

	matrixDataModule.setModuleParamaters(deviceContext, worldMatrix, viewMatrix, projectionMatrix);

		miniMapDataType* miniMapPtr;
		deviceContext->Map(miniMapBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		miniMapPtr = (miniMapDataType*)mappedResource.pData;
		miniMapPtr->playerIconColour = playerIconColour;
		miniMapPtr->greyScaleValues = greyScaleValues;
		miniMapPtr->playerIconRadius = playerIconRadius;
		miniMapPtr->cameraPos = screenPos;

		deviceContext->Unmap(miniMapBuffer.Get(), 0);

	ID3D11Buffer* miniMapBufferPtr = miniMapBuffer.Get();
	deviceContext->PSSetConstantBuffers(0, 1, &miniMapBufferPtr);

	// Set shader texture and sampler resource in the pixel shader.
	deviceContext->PSSetShaderResources(0, 1, &texture);
	ID3D11SamplerState* sampleStatePtr = sampleState.Get();
	deviceContext->PSSetSamplers(0, 1, &sampleStatePtr);
}

void MiniMapTextureShader::ImGuiMenu()
{
	if (ImGui::CollapsingHeader("MiniMapControl"))
	{
		if (ImGui::TreeNode("ProjectionMatrixSettings")) {
			bool matrixUpdated = false;

			if (ImGui::SliderFloat("orthoWidth: ", &miniMapWidth, 0.0f, 1000.f)) { matrixUpdated = true; }
			if (ImGui::SliderFloat("orthoHeight: ", &miniMapHeight, 0.0f, 1000.f)) { matrixUpdated = true; }
			if (ImGui::SliderFloat("orthoNearZ: ", &miniMapNearZ, 0.1f, 1000.f)) { matrixUpdated = true; }
			if (ImGui::SliderFloat("orthoFarZ: ", &miniMapFarZ, 0.1f, 1000.f)) { matrixUpdated = true; }

			if (matrixUpdated) {
				//Calulcate the orthographic projection matrix
				orthographicProjectionMatrix = XMMatrixOrthographicLH(miniMapWidth, miniMapHeight, miniMapNearZ, miniMapFarZ);
			}
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("Player Icon Control")) {
			if (ImGui::TreeNode("Player Icon Colour")) {
				if (ImGui::ColorPicker4("Icon Colour", &playerIconColour.x)) {}
				ImGui::TreePop();
			}
			if (ImGui::SliderFloat("Icon Radius", &playerIconRadius, 1.f, 50.f)) {}
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("GreyScale Value")) {
			if (ImGui::SliderFloat("Red Weight", &greyScaleValues.x, 0.0f, 1.0f)) {}
			if (ImGui::SliderFloat("Green Weight", &greyScaleValues.y, 0.0f, 1.0f)) {}
			if (ImGui::SliderFloat("Blue Weight", &greyScaleValues.z, 0.0f, 1.0f)) {}
			ImGui::TreePop();
		}
	}
}