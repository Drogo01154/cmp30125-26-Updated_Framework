// Lab1.cpp
// Lab 1 example, simple coloured triangle mesh
#include "App1.h"
#include "FinalPass.h"
#include "ShadowPass.h"
#include "TextureShader.h"
#include "ShadowShader.h"
#include "DepthShader.h"
#include "HeightDepthShader.h"
#include "BasicLightShader.h"
#include "LightPass.h"
#include "HeightMapShadowShader.h"
#include "HeightMapLightShader.h"

#include "GenericShader.h"
#include "ConstantBufferModule.h"

#include "BloomCombinePass.h"
#include "BlurPass.h"


App1::App1()
{
	UIConstants::InitialiseSystem();
}

//Function for storing constructors to create different module types
void App1::addModuleConstructors(HWND hwnd, ID3D11Device* device) {
	//Matrix data module to send matrix data to vertex shader
	shaderMgr->AddShaderModule<MatrixDataModule>("MatrixDataModule", device, hwnd, instanceMgr.get());

	//Texture data modules to send albedo colour to shaders
	shaderMgr->AddShaderModule<TextureDataModule>("TextureDataModule1", device, hwnd);
	shaderMgr->AddShaderModule<TextureDataModule>("TextureDataModule2", device, hwnd);
	shaderMgr->AddShaderModule<TextureDataModule>("TextureDataModule3", device, hwnd);

	//Lights data module to send light information to shaders
	shaderMgr->AddShaderModule<LightsDataModule>("LightsDataModule", device, hwnd, instanceMgr.get());
	//Another light module to send light specific data to depth buffers
	shaderMgr->AddShaderModule<LightDepthDataBuffer>("LightDepthDataModule", device, hwnd);
	//Material module to send material data to shaders
	shaderMgr->AddShaderModule<MaterialDataModule>("MaterialDataModule", device, hwnd);
	//Camera module to send camera and world data to shaders
	shaderMgr->AddShaderModule<CameraDataModule>("CameraDataModule", device, hwnd, instanceMgr.get(), sceneGraph.get());
	//Shadow map module to send shadow and cube maps as well as viewProj matrices to shaders
	shaderMgr->AddShaderModule<ShadowMapDataModule>("ShadowMapDataModule", device, hwnd);
	//Height map data module to send height map data to shaders
	shaderMgr->AddShaderModule<HeightMapDataModule>("HeightMapDataModule", device, hwnd);

	//Sampler specifically for sampling render textures	
	shaderMgr->AddShaderModule<SamplerDataModule>("RenderTextureSamplerModule", device, hwnd, D3D11_TEXTURE_ADDRESS_CLAMP);
	//Sampler specifically for sampling material and heightmap textures
	shaderMgr->AddShaderModule<SamplerDataModule>("MaterialTextureSamplerModule", device, hwnd);
	//Samplers for sampling cube and shadow maps
	shaderMgr->AddShaderModule<SamplerDataModule>("ShadowMapSamplerModule", device, hwnd, D3D11_TEXTURE_ADDRESS_BORDER, D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR, 1, D3D11_COMPARISON_LESS_EQUAL);
	shaderMgr->AddShaderModule<SamplerDataModule>("CubeMapSamplerModule", device, hwnd, D3D11_TEXTURE_ADDRESS_CLAMP, D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR, 1, D3D11_COMPARISON_LESS_EQUAL);

	shaderMgr->AddShaderModule<SamplerDataModule>("HeightMapSamplerModule", device, hwnd, D3D11_TEXTURE_ADDRESS_CLAMP, D3D11_FILTER_MIN_MAG_MIP_LINEAR);

	shaderMgr->AddShaderModule<ConstantDataModule<BloomBufferType>>("BloomDataModule", device, hwnd);
	shaderMgr->AddShaderModule<ConstantDataModule<BlurBufferType>>("BlurModule", device, hwnd);
}

//Function for storing constructors of different shader types
void App1::addShaderConstructors(HWND hwnd, ID3D11Device* device) {

	//Depth shaders
	shaderMgr->addGeometryShader<DepthShader>("DepthShader", shaderMgr.get(), device, hwnd);
	shaderMgr->addGeometryShader<HeightDepthShader>("HeightDepthShader", shaderMgr.get(), device, hwnd);

	//Shadow map shaders
	shaderMgr->addGeometryShader<ShadowShader>("ShadowShader", shaderMgr.get(), device, hwnd);
	shaderMgr->addGeometryShader<HeightMapShadowShader>("HeightMapShadowShader", shaderMgr.get(), device, hwnd);
	shaderMgr->addGeometryShader<HeightMapLightShader>("HeightMapLightShader", shaderMgr.get(), device, hwnd);

	shaderMgr->addGeometryShader<BasicLightShader>("BasicLightShader", shaderMgr.get(), device, hwnd);

	std::vector<resourceData> shaderInputs;
	shaderInputs.resize(4);
	shaderInputs[0] = resourceData("MatrixDataModule", { {D3D11_SHADER_VERSION_TYPE::D3D11_SHVER_VERTEX_SHADER, 0} });
	shaderInputs[1] = resourceData("TextureDataModule1", { {D3D11_SHADER_VERSION_TYPE::D3D11_SHVER_PIXEL_SHADER, 0} });
	shaderInputs[2] = resourceData("RenderTextureSamplerModule", { {D3D11_SHADER_VERSION_TYPE::D3D11_SHVER_PIXEL_SHADER, 0} });
	shaderInputs[3] = resourceData("BlurModule", { {D3D11_SHADER_VERSION_TYPE::D3D11_SHVER_PIXEL_SHADER, 0} });

	//Add Post process shaders
	shaderMgr->addPassShader<GenericShader>("GausseanShader", shaderMgr.get(), device, hwnd,
		shaderData(L"GausseanBlur_ps.cso", L"texture_vs.cso"), shaderInputs);

	shaderInputs.erase(shaderInputs.begin() + 3);

	shaderInputs.push_back(resourceData("BloomDataModule", { {D3D11_SHVER_PIXEL_SHADER, 0} }));

	//Add Post process shaders
	shaderMgr->addPassShader<GenericShader>("BloomThresholdShader", shaderMgr.get(), device, hwnd,
		shaderData(L"BloomThreshold_ps.cso", L"texture_vs.cso"), shaderInputs);

	shaderInputs.push_back(resourceData("TextureDataModule2", { {D3D11_SHADER_VERSION_TYPE::D3D11_SHVER_PIXEL_SHADER, 1} }));

	//Add Post process shaders
	shaderMgr->addPassShader<GenericShader>("BloomCombineShader", shaderMgr.get(), device, hwnd,
		shaderData(L"BloomCombine_ps.cso", L"texture_vs.cso"), shaderInputs);

	//Add final pass shader
	shaderMgr->addPassShader<TextureShader>("TextureShader", shaderMgr.get(), device, hwnd);
}

void App1::initShadowPasses(HWND hwnd, int screenWidth, int screenHeight) {

	passMgr->AddPassConstructionFunction<DepthPass>("DepthPass", {}, DepthPassInput(instanceMgr.get(), shaderMgr.get(), renderer, 2048, 2048));
	
	passMgr->AddPassConstructionFunction<ShadowPass>("ShadowPass", { "DepthPass" }, shaderMgr.get(),
		instanceMgr.get(), renderer->getDevice(), renderer,
		RenderTextureData(screenWidth, screenHeight, SCREEN_NEAR, SCREEN_DEPTH));
}
void App1::initBasicLightPasses(HWND hwnd, int screenWidth, int screenHeight) {
	passMgr->AddPassConstructionFunction<LightPass>("LightPass", {}, shaderMgr.get(),
		instanceMgr.get(), renderer->getDevice(), renderer,
		RenderTextureData(screenWidth, screenHeight, SCREEN_NEAR, SCREEN_DEPTH));

}
void App1::initBloompasses(HWND hwnd, int screenWidth, int screenHeight, const char* previousPass) {
	passMgr->AddPassConstructionFunction<ThresholdPass>("ThresholdPass", { previousPass }, instanceMgr.get(), shaderMgr.get(), renderer,
		RenderTextureData(screenWidth, screenHeight, SCREEN_NEAR, SCREEN_DEPTH), OrthoMeshData(screenWidth, screenHeight, 0.0f, 0.0f));
	passMgr->AddPassConstructionFunction<BlurPass>("BlurPass", {"ThresholdPass"}, instanceMgr.get(), shaderMgr.get(), renderer,
		RenderTextureData(screenWidth, screenHeight, SCREEN_NEAR, SCREEN_DEPTH), OrthoMeshData(screenWidth, screenHeight, 0.0f, 0.0f));
	passMgr->AddPassConstructionFunction<BloomCombinePass>("BlurCombinePass", { previousPass, "BlurPass"}, instanceMgr.get(), shaderMgr.get(), renderer,
		RenderTextureData(screenWidth, screenHeight, SCREEN_NEAR, SCREEN_DEPTH), OrthoMeshData(screenWidth, screenHeight, 0.0f, 0.0f));
}

void App1::initBasicLightsPipeline(HWND hwnd, int screenWidth, int screenHeight) {
	initBasicLightPasses(hwnd, screenWidth, screenHeight);
	passMgr->AddPassConstructionFunction<FinalRenderPass>("FinalPass", { "LightPass" }, renderer, shaderMgr.get(), instanceMgr.get(), OrthoMeshData(screenWidth, screenHeight, 0.0f, 0.0f));
}
void App1::initShadowLightsPipeline(HWND hwnd, int screenWidth, int screenHeight) {
	initShadowPasses(hwnd, screenWidth, screenHeight);
	passMgr->AddPassConstructionFunction<FinalRenderPass>("FinalPass", { "ShadowPass" }, renderer, shaderMgr.get(), instanceMgr.get(), OrthoMeshData(screenWidth, screenHeight, 0.0f, 0.0f));

}

void App1::initBloomLightsPipeline(HWND hwnd, int screenWidth, int screenHeight) {
	initBasicLightPasses(hwnd, screenWidth, screenHeight);
	initBloompasses(hwnd, screenWidth, screenHeight, "LightPass");
	passMgr->AddPassConstructionFunction<FinalRenderPass>("FinalPass", { "BlurCombinePass" }, renderer, shaderMgr.get(), instanceMgr.get(), OrthoMeshData(screenWidth, screenHeight, 0.0f, 0.0f));
}
void App1::initBloomShadowPipeline(HWND hwnd, int screenWidth, int screenHeight) {
	initShadowPasses(hwnd, screenWidth, screenHeight);
	initBloompasses(hwnd, screenWidth, screenHeight, "ShadowPass");
	passMgr->AddPassConstructionFunction<FinalRenderPass>("FinalPass", { "BlurCombinePass" }, renderer, shaderMgr.get(), instanceMgr.get(), OrthoMeshData(screenWidth, screenHeight, 0.0f, 0.0f));
}

void App1::selectPipeline(Pipelines pipeline) {
	passMgr->clearActivePasses();
	switch (pipeline) {
	case Pipelines::BASICLIGHTING:
		initBasicLightsPipeline(wnd, sWidth, sHeight);
		break;
	case Pipelines::BASICLIGHTINGWITHBLOOM:
		initBloomLightsPipeline(wnd, sWidth, sHeight);
		break;
	case Pipelines::SHADOWS:
		initShadowLightsPipeline(wnd, sWidth, sHeight);
		break;
	case Pipelines::SHADOWSWITHBLOOM:
		initBloomShadowPipeline(wnd, sWidth, sHeight);
		break;
	default:
		throw std::runtime_error("Error: pipeline does not exist!");
	}
	selectedPipeline = static_cast<int>(pipeline);
	passMgr->InitPass("FinalPass");
	activePipeline = pipeline;
	shaderMgr->SetModuleDirtyflags(DirtyModuleFlags::ALL);
}

void App1::init(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight, Input *in, bool VSYNC, bool FULL_SCREEN)
{	
	
	saveAsBson = true;

	// Call super/parent init function (required!)
	BaseApplication::init(hinstance, hwnd, screenWidth, screenHeight, in, VSYNC, FULL_SCREEN);
	
	addModuleConstructors(hwnd, renderer->getDevice());
	addShaderConstructors(hwnd, renderer->getDevice());

	selectPipeline(Pipelines::SHADOWSWITHBLOOM);

	selectedScene = -1;
	
	sceneName = "Default";
	strcpy_s(SceneNameBuffer, sizeof(SceneNameBuffer), sceneName.c_str());

	//sceneGraph->createBaseScene();

	loadScene("Main Scene");
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
	ImGui::Text(("Scene: " + sceneName).c_str());
	ImGui::Text("FPS: %.2f", timer->getFPS());

	std::shared_ptr<Camera> cam = instanceMgr->getActiveCamera()->camera;
	const XMFLOAT3& cameraPos = cam->getGlobalPosition();
	const XMFLOAT3& cameraDir = cam->getGlobalDirection();
	std::string posText = "Camera Position: X: " + std::to_string(cameraPos.x) + " Y: " + std::to_string(cameraPos.y) + " Z: " + std::to_string(cameraPos.z);
	std::string dirText = "Camera Direction: X: " + std::to_string(cameraDir.x) + " Y: " + std::to_string(cameraDir.y) + " Z: " + std::to_string(cameraDir.z);
	ImGui::Text(posText.c_str());
	ImGui::Text(dirText.c_str());

	if (ImGui::CollapsingHeader("Scene Menu")) {
		if (ImGui::InputText("Scene Name: ", SceneNameBuffer, sizeof(SceneNameBuffer))) {

		}
		if (ImGui::Button(saveAsBson ? "As Bson" : "As Json")) { saveAsBson = !saveAsBson; }
		if (ImGui::Button("Save Scene")) {
			std::string sceneName(SceneNameBuffer);
			if(!sceneName.empty()) {
				saveScene(sceneName);
			}
		}

		const std::vector<const char*>* sceneList = FileHandler::get().getSceneList();
		ImGui::Combo("Select Scene", &selectedScene, sceneList->data(), sceneList->size());
		if (ImGui::Button("LoadScene") && selectedScene >= 0) {
			std::string sceneName(sceneList->at(selectedScene));
			loadScene(sceneName);
		}

		if (ImGui::Button("Reset Scene")) {
			passMgr->clearActivePasses();
			sceneGraph->resetScene();
			selectPipeline(activePipeline);
		}
	}


	materialMgr->imGuiRender();

	sceneGraph->imGuiRender();
	if (ImGui::CollapsingHeader("Pipeline Settings")) {
		if (ImGui::Combo("Select Pipeline", &selectedPipeline, PipelinesStrings.data(), PipelinesStrings.size())) {
			selectPipeline(static_cast<Pipelines>(selectedPipeline));
		}
		passMgr->ImGuiRender();
	}
	

	// Render UI
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

void App1::saveScene(const std::string& sceneName) {
	nlohmann::json inputJSON;
	passMgr->toJson(inputJSON);
	passMgr->clearActivePasses();
	sceneGraph->to_json(inputJSON);
	inputJSON["activePipeline"] = activePipeline;

	if (FileHandler::get().saveSceneJson(sceneName, inputJSON, saveAsBson)) {
		this->sceneName = sceneName;
	}
	selectPipeline(activePipeline);
	passMgr->fromJson(inputJSON);
	
}

void App1::loadScene(const std::string& sceneName) {
	passMgr->clearActivePasses();
	nlohmann::json outputJSON;
	if (FileHandler::get().loadSceneJson(sceneName, outputJSON)) {
		activePipeline = outputJSON.at("activePipeline").get<Pipelines>();
		sceneGraph->from_json(outputJSON);
		this->sceneName = sceneName;
		strcpy_s(SceneNameBuffer, sizeof(SceneNameBuffer), sceneName.c_str());
	}
	selectPipeline(activePipeline);
	passMgr->fromJson(outputJSON);
}
