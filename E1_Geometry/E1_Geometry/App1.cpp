// Lab1.cpp
// Lab 1 example, simple coloured triangle mesh
#include "App1.h"
#include "FinalPass.h"
#include "ShadowPass.h"
#include "TextureShader.h"
#include "ShadowShader.h"
#include "CMDepthShader.h"
#include "SMDepthShader.h"
#include "TexturedLightShader.h"
#include "LightPass.h"

App1::App1()
{
	UIConstants::InitialiseSystem();
}

void App1::initShadows(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight) {
	//Add modules
	shaderMgr->AddShaderModule<MatrixDataModule>("MatrixDataModule", renderer->getDevice(), hwnd, instanceMgr.get());
	shaderMgr->AddShaderModule<TextureDataModule>("TextureDataModule", renderer->getDevice(), hwnd);
	shaderMgr->AddShaderModule<LightsDataModule>("LightsDataModule", renderer->getDevice(), hwnd, instanceMgr.get());
	shaderMgr->AddShaderModule<PointLightDataModule>("PointLightDataModule", renderer->getDevice(), hwnd);
	shaderMgr->AddShaderModule<MaterialDataModule>("MaterialDataModule", renderer->getDevice(), hwnd);
	shaderMgr->AddShaderModule<CameraDataModule>("CameraDataModule", renderer->getDevice(), hwnd, instanceMgr.get(), sceneGraph.get());
	shaderMgr->AddShaderModule<ShadowMapDataModule>("ShadowMapDataModule", renderer->getDevice(), hwnd);


	shaderMgr->AddShaderModule<SamplerDataModule>("RenderTextureSamplerModule", renderer->getDevice(), hwnd, D3D11_TEXTURE_ADDRESS_CLAMP);
	shaderMgr->AddShaderModule<SamplerDataModule>("MaterialTextureSamplerModule", renderer->getDevice(), hwnd);
	shaderMgr->AddShaderModule<SamplerDataModule>("ShadowMapSamplerModule", renderer->getDevice(), hwnd, D3D11_TEXTURE_ADDRESS_BORDER, D3D11_FILTER_MIN_MAG_MIP_POINT);
	shaderMgr->AddShaderModule<SamplerDataModule>("CubeMapSamplerModule", renderer->getDevice(), hwnd, D3D11_TEXTURE_ADDRESS_CLAMP, D3D11_FILTER_MIN_MAG_MIP_POINT);


	shaderMgr->addGeometryShader<SMDepthShader>("SMDepthShader", shaderMgr.get(), renderer->getDevice(), hwnd);
	shaderMgr->addGeometryShader<CMDepthShader>("CMDepthShader", shaderMgr.get(), renderer->getDevice(), hwnd);
	shaderMgr->addGeometryShader<ShadowShader>("ShadowShader", shaderMgr.get(), renderer->getDevice(), hwnd);

	//Add pass shader
	shaderMgr->addPassShader<TextureShader>("TextureShader", shaderMgr.get(), renderer->getDevice(), hwnd);

	passMgr->AddPassConstructionFunction<DepthPass>("DepthPass", {}, DepthPassInput(instanceMgr.get(), shaderMgr.get(), renderer, 1024, 1024));

	passMgr->AddPassConstructionFunction<ShadowPass>("ShadowPass", { "DepthPass" }, shaderMgr.get(),
	instanceMgr.get(), renderer->getDevice(), renderer,
	RenderTextureData(screenWidth, screenHeight, SCREEN_NEAR, SCREEN_DEPTH));


	passMgr->AddPassConstructionFunction<FinalRenderPass>("FinalPass", { "ShadowPass" }, renderer, shaderMgr.get(), instanceMgr.get(), OrthoMeshData(screenWidth, screenHeight, 0.0f, 0.0f));

	passMgr->InitPass("FinalPass");
}
void App1::initBasicLights(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight) {
	//Add modules
	shaderMgr->AddShaderModule<MatrixDataModule>("MatrixDataModule", renderer->getDevice(), hwnd, instanceMgr.get());
	shaderMgr->AddShaderModule<TextureDataModule>("TextureDataModule", renderer->getDevice(), hwnd);
	shaderMgr->AddShaderModule<LightsDataModule>("LightsDataModule", renderer->getDevice(), hwnd, instanceMgr.get());
	shaderMgr->AddShaderModule<MaterialDataModule>("MaterialDataModule", renderer->getDevice(), hwnd);
	shaderMgr->AddShaderModule<CameraDataModule>("CameraDataModule", renderer->getDevice(), hwnd, instanceMgr.get(), sceneGraph.get());


	shaderMgr->AddShaderModule<SamplerDataModule>("RenderTextureSamplerModule", renderer->getDevice(), hwnd, D3D11_TEXTURE_ADDRESS_CLAMP);
	shaderMgr->AddShaderModule<SamplerDataModule>("MaterialTextureSamplerModule", renderer->getDevice(), hwnd);

	//Add geometry Shader
	shaderMgr->addGeometryShader<TexturedLightShader>("BasicLightShader", shaderMgr.get(), renderer->getDevice(), hwnd);

	//Add pass shader
	shaderMgr->addPassShader<TextureShader>("TextureShader", shaderMgr.get(), renderer->getDevice(), hwnd);

	passMgr->AddPassConstructionFunction<LightPass>("LightPass", {}, shaderMgr.get(),
		instanceMgr.get(), renderer->getDevice(), renderer,
		RenderTextureData(screenWidth, screenHeight, SCREEN_NEAR, SCREEN_DEPTH));


	passMgr->AddPassConstructionFunction<FinalRenderPass>("FinalPass", { "LightPass" }, renderer, shaderMgr.get(), instanceMgr.get(), OrthoMeshData(screenWidth, screenHeight, 0.0f, 0.0f));

	passMgr->InitPass("FinalPass");
}

void App1::init(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight, Input *in, bool VSYNC, bool FULL_SCREEN)
{	
	// Call super/parent init function (required!)
	BaseApplication::init(hinstance, hwnd, screenWidth, screenHeight, in, VSYNC, FULL_SCREEN);
	
	//initBasicLights(hinstance, hwnd, screenWidth, screenHeight);
	initShadows(hinstance, hwnd, screenWidth, screenHeight);
}


App1::~App1()
{
	// Run base application deconstructor
	BaseApplication::~BaseApplication();
}


bool App1::frame()
{
	UIConstants::UpdateConstants(sWidth, sHeight);

	bool result;

	result = BaseApplication::frame();
	if (!result)
	{
		return false;
	}
	
	// Render the graphics.
	result = render();
	if (!result)
	{
		return false;
	}
	
	return true;
}

bool App1::render()
{
	// Get matrices
	if (shaderMgr->isModuleDirtyflagSet(DirtyModuleFlags::CAMERA)) {
		instanceMgr->getActiveCamera()->camera->update();
	}

	passMgr->RenderAll(renderer->getDeviceContext(), renderer->getDevice());

	shaderMgr->ResetModuleDirtyflags();

	// Render GUI
	gui();

	// Present the rendered scene to the screen.
	renderer->endScene();
	return true;
}

void App1::gui()
{
	// Force turn off unnecessary shader stages.
	renderer->getDeviceContext()->GSSetShader(NULL, NULL, 0);
	renderer->getDeviceContext()->HSSetShader(NULL, NULL, 0);
	renderer->getDeviceContext()->DSSetShader(NULL, NULL, 0);

	// Build UI
	ImGui::Text("FPS: %.2f", timer->getFPS());
	ImGui::Checkbox("Wireframe mode", &wireframeToggle);

	std::shared_ptr<Camera> cam = instanceMgr->getActiveCamera()->camera;
	const XMFLOAT3& cameraPos = cam->getGlobalPosition();
	const XMFLOAT3& cameraDir = cam->getGlobalDirection();
	std::string posText = "Camera Position: X: " + std::to_string(cameraPos.x) + " Y: " + std::to_string(cameraPos.y) + " Z: " + std::to_string(cameraPos.z);
	std::string dirText = "Camera Direction: X: " + std::to_string(cameraDir.x) + " Y: " + std::to_string(cameraDir.y) + " Z: " + std::to_string(cameraDir.z);
	ImGui::Text(posText.c_str());
	ImGui::Text(dirText.c_str());

	materialMgr->imGuiRender();

	sceneGraph->imGuiRender();

	// Render UI
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}
