// Lab1.cpp
// Lab 1 example, simple coloured triangle mesh
#include "App1.h"
#include "ImGuiHelpers.h"
#include "MathHelpers.h"

App1::App1()
{

	cubeMesh = nullptr;
	sphereMesh = nullptr;
	sphereMesh = nullptr;
	texturedLightShader = nullptr;
	textureShader = nullptr;
	shapeMaterial = nullptr;
	UIConstants::InitialiseSystem();

	ambientLight = { 0.0f, 0.0f, 0.0f, 1.f };
	selectedLight = -1;

	miniMapCamera = nullptr;
}

void App1::init(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight, Input *in, bool VSYNC, bool FULL_SCREEN)
{	
	// Call super/parent init function (required!)
	BaseApplication::init(hinstance, hwnd, screenWidth, screenHeight, in, VSYNC, FULL_SCREEN);

	// Create Mesh object and shader object
	// Cube mesh is the geometry in the scene.
	// Ortho mesh is the geometry we render the result to.
	textureMgr->loadTexture(L"brick", L"res/brick1.dds");
	cubeMesh = make_shared<CubeMesh>(renderer->getDevice(), renderer->getDeviceContext());
	sphereMesh = make_shared<SphereMesh>(renderer->getDevice(), renderer->getDeviceContext());
	orthoMeshTR = make_shared<OrthoMesh>(renderer->getDevice(), renderer->getDeviceContext(), screenWidth / 4, screenHeight / 4, screenWidth / 2.7f, screenHeight / 2.7f);
	orthoMeshMain = make_shared<OrthoMesh>(renderer->getDevice(), renderer->getDeviceContext(), screenWidth, screenHeight, 0.0f, 0.0f);

	// Initialise shaders
	// Lighting for normal rendering
	// Texture to output the result (we don't need to do lighting again, as it has already be calculated)
	texturedLightShader = std::make_shared<TexturedLightShader>(renderer->getDevice(), textureMgr, hwnd);
	textureShader = std::make_shared<TextureShader>(renderer->getDevice(), hwnd);
	miniMapTextureShader = std::make_shared<MiniMapTextureShader>(renderer->getDevice(), hwnd, screenWidth, screenHeight);


	// Build RenderTexture, this will be our alternative render target.
	renderTextureTR = make_shared<RenderTexture>(renderer->getDevice(), screenWidth, screenHeight, SCREEN_NEAR, SCREEN_DEPTH);
	renderTextureMain = make_shared<RenderTexture>(renderer->getDevice(), screenWidth, screenHeight, SCREEN_NEAR, SCREEN_DEPTH);

	shapeMaterial = make_shared<Material>();
	// Create textured material
	shapeMaterial->shader = texturedLightShader;
	shapeMaterial->baseColour = XMFLOAT4(1.f, 1.f, 1.f, 1.f);
	shapeMaterial->specularColour = XMFLOAT4(1.f, 1.f, 1.f, 1.f);
	shapeMaterial->specularPower = 32.f;
	shapeMaterial->texture = L"brick";

	// Create light
	lights.emplace_back(make_shared<Light>(Light(lightTypes::directional)));
	selectedLight = lights.size() - 1;
	lights[0]->setDiffuseColour(1.0f, 1.0f, 1.0f, 1.0f);
	lights[0]->setDirection(0.7f, 0.0f, 0.7f);

	//Create second camera
	miniMapCamera = std::make_shared<Camera>();
	miniMapCamera->setRotation(90.0f, 0.0f, 0.0f);
	miniMapCamera->setPosition(0.0f, 10.f, 0.0f);
	miniMapCamera->update();


	//camera->setPosition(35.f, 20.f, 0.f);
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
	camera->update();

	// Render first pass to top left render texture
	firstPass();

	// Render second pass to top right render texture
	secondPass();

	// Render final pass to frame buffer
	finalPass();

	return true;
}

void App1::firstPass()
{
	// Set the render target to be the render to terxture and clear it
	renderTextureTR->setRenderTarget(renderer->getDeviceContext());
	renderTextureTR->clearRenderTarget(renderer->getDeviceContext(), 0.39f, 0.58f, 0.92f, 1.0f);

	// Get Matrices
	XMMATRIX worldMatrix = renderer->getWorldMatrix();
	XMMATRIX viewMatrix = miniMapCamera->getViewMatrix();
	XMMATRIX projectionMatrix = miniMapTextureShader->getOrthographicMatrix();

	// Render normal scene, with light shader set.
	cubeMesh->sendData(renderer->getDeviceContext());
	texturedLightShader->setShaderParamaters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, shapeMaterial, lights, ambientLight, miniMapCamera.get());
	texturedLightShader->render(renderer->getDeviceContext(), cubeMesh->getIndexCount());

	worldMatrix *= XMMatrixTranslation(2.0f, 0.0f, 5.0f);

	sphereMesh->sendData(renderer->getDeviceContext());
	Camera* c = miniMapCamera.get();

	texturedLightShader->setShaderParamaters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, shapeMaterial, lights, ambientLight, miniMapCamera.get());
	texturedLightShader->render(renderer->getDeviceContext(), sphereMesh->getIndexCount());

	// Reset the render target back to the original back buffer and not the render to texture anymore.
	renderer->setBackBufferRenderTarget();
}

void App1::secondPass()
{

	// Set the render target to be the render to terxture and clear it
	renderTextureMain->setRenderTarget(renderer->getDeviceContext());
	renderTextureMain->clearRenderTarget(renderer->getDeviceContext(), 0.39f, 0.58f, 0.92f, 1.0f);

	XMMATRIX worldMatrix = renderer->getWorldMatrix();
	XMMATRIX viewMatrix = camera->getViewMatrix();
	XMMATRIX projectionMatrix = renderer->getProjectionMatrix();

	// Render normal scene, with light shader set.
	cubeMesh->sendData(renderer->getDeviceContext());
	texturedLightShader->setShaderParamaters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, shapeMaterial, lights, ambientLight, camera);
	texturedLightShader->render(renderer->getDeviceContext(), cubeMesh->getIndexCount());

	worldMatrix *= XMMatrixTranslation(2.0f, 0.0f, 5.0f);

	sphereMesh->sendData(renderer->getDeviceContext());
	texturedLightShader->setShaderParamaters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, shapeMaterial, lights, ambientLight, camera);
	texturedLightShader->render(renderer->getDeviceContext(), sphereMesh->getIndexCount());

	// Reset the render target back to the original back buffer and not the render to texture anymore.
	renderer->setBackBufferRenderTarget();
}
 
VOID App1::finalPass()
{
	// Clear the scene. (default blue colour)
	renderer->beginScene(1.0f, 1.0f, 1.0f, 1.0f);

	// Get matrices
	camera->update();
	XMMATRIX worldMatrix = renderer->getWorldMatrix();
	XMMATRIX viewMatrix = camera->getViewMatrix();
	XMMATRIX projectionMatrix = renderer->getProjectionMatrix();

	// RENDER THE RENDER TEXTURE SCENE
	// Requires 2D rendering and an ortho mesh.
	renderer->setZBuffer(false);
	XMMATRIX orthoMatrix = renderer->getOrthoMatrix();  // ortho matrix for 2D rendering
	XMMATRIX orthoViewMatrix = camera->getOrthoViewMatrix();	// Default camera position for orthographic rendering


	orthoMeshMain->sendData(renderer->getDeviceContext());
	textureShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, orthoViewMatrix, orthoMatrix, renderTextureMain->getShaderResourceView());
	textureShader->render(renderer->getDeviceContext(), orthoMeshMain->getIndexCount());

	orthoMeshTR->sendData(renderer->getDeviceContext());
	miniMapTextureShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, orthoViewMatrix, orthoMatrix, renderTextureTR->getShaderResourceView(), camera, miniMapCamera.get());
	miniMapTextureShader->render(renderer->getDeviceContext(), orthoMeshTR->getIndexCount());

	renderer->setZBuffer(true);

	// Render GUI
	gui();

	// Present the rendered scene to the screen.
	renderer->endScene();
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

	XMFLOAT3 camPos = camera->getPosition();
	std::string outputPos = "X: " + std::to_string(camPos.x) + " Y: " + std::to_string(camPos.y) + " Z: " + std::to_string(camPos.z);
	ImGui::Text(outputPos.c_str());

	/*
	if (ImGui::SliderInt("Resolution: ", &resolution, 2, 1000)) {
		plane.reset(); // Delete Plane

		//Make new plane
		plane = make_shared<PlaneMesh>(renderer->getDevice(), renderer->getDeviceContext(), resolution);
	}
	*/

	//Light ImGui

	miniMapTextureShader->ImGuiMenu();

	if (ImGui::CollapsingHeader("Materials")) {

		if (ImGui::TreeNode("Shape Material")) {
			if (ImGui::TreeNode("Colour: ## 0")) {
				if(ImGui::ColorPicker4("Base Colour: ## 0", &shapeMaterial->baseColour.x)) {}
				ImGui::TreePop();
			}

			if (ImGui::TreeNode("Specular Colour: ## 0")) {
				if (ImGui::ColorPicker4("Ambient Light: ", &shapeMaterial->specularColour.x)) {}
				ImGui::TreePop();
			}
			if (ImGui::SliderFloat("Specular Power: ## 0", &shapeMaterial->specularPower, 1.f, 1000.f)) {}
			ImGui::TreePop();
		}
	}


	//Light Data
	if (ImGui::CollapsingHeader("Lighting")) {

		//Ambient Light Settings
		if (ImGui::TreeNode("Ambient Light")) {
			if (ImGui::ColorPicker4("Ambient Light: ", &ambientLight.x)) {}
			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Spawn Lights")) {
			for (int i = 0; i < 3; i++) {
				if (ImGui::Button(LightTypeStrings[i].c_str()))
				{
					lights.emplace_back(make_shared<Light>(Light(static_cast<lightTypes>(i))));
					selectedLight = lights.size() - 1;
				}
				if (i < 2) { ImGui::SameLine(); }
			}
			ImGui::TreePop();
		}

		if ((lights.size() > 0) && ImGui::TreeNode("Existing Lights: ")) {

			if ((lights.size() > 1) && ArrowIterators("Selected_Light", selectedLight, (uint32_t)lights.size())) {}

			if (selectedLight >= 0) {
				std::shared_ptr<Light> light = lights[selectedLight];

				//Light Settings
				lightTypes type = light->getType();

				ImGui::Text(("Light Type: " + LightTypeStrings[type]).c_str());

				//Lights Diffuse Colour setting
				XMFLOAT4 diffuse = light->getDiffuseColour();
				if (ImGui::TreeNode("Light Diffuse Colour")) {
					if (ImGui::ColorPicker4("Light Diffuse Colour: ", &diffuse.x)) {
						light->setDiffuseColour(diffuse);
					}
					ImGui::TreePop();
				}

				if (type != lightTypes::directional)
				{
					if (ImGui::TreeNode("Attenuation")) {
						XMFLOAT4 attenuation = light->getAttenuation();
						if (ImGui::DragFloat("Constant ", &attenuation.x, 0.1f, 1.0f, 1000.f, "%.2f")) {
							light->setAttenuation(attenuation);
						}
						if (ImGui::DragFloat("Linear ", &attenuation.y, 0.01f, 0.01f, 0.15f, "%.2f")) {
							light->setAttenuation(attenuation);
						}
						if (ImGui::DragFloat("Quadratic ", &attenuation.z, 0.0001f, 0.0001f, 0.2f, "%.4f")) {
							light->setAttenuation(attenuation);
						}
						if (ImGui::DragFloat("Falloff ", &attenuation.w, 0.1f, 0.01f, 1000.f, "%.2f")) {
							light->setAttenuation(attenuation);
						}
						ImGui::TreePop();
					}

					//Lights Transform Settings
					XMFLOAT3 position = light->getPosition();
					if (ImGui::DragFloat3("Light Position: ", &position.x, 1.f)) {
						light->setPosition(position);
					}
				}

				if (type != lightTypes::point) {
					XMFLOAT3 direction = ToDegrees(light->getDirectionEuler());
					if (ImGui::DragFloat3("Light Direction Euler: ", &direction.x, 0.1f)) {
						light->setDirectionEuler(ToRadians(direction));
					}
				}

				if (type == lightTypes::spot) {
					float test = light->getInnerCone();
					float innerCone = light->getInnerCone();
					if (ImGui::SliderAngle("Inner Cone: ", &innerCone, 0.f, 90.f)) {
						light->setInnerCone(innerCone);
					}
					float outerCone = light->getOuterCone();
					if (ImGui::SliderAngle("Outer Cone: ", &outerCone, 0.f, 90.f)) {
						light->setOuterCone(outerCone);
					}
				}

				if (ImGui::Button("Delete Light")) {
					lights.erase(lights.begin() + selectedLight);
					--selectedLight;
					if (selectedLight < 0) { selectedLight = lights.size() - 1; }
				}
			}
			ImGui::TreePop();
		}
	}

	// Render UI
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}
