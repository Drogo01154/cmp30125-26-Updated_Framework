// Lab1.cpp
// Lab 1 example, simple coloured triangle mesh
#include "App1.h"
#include "ImGuiHelpers.h"
#include "MathHelpers.h"

App1::App1()
{
	plane = nullptr;
	manipulationShader = nullptr;
	planeMaterial = nullptr;
	UIConstants::InitialiseSystem();

	ambientLight = { 0.1f, 0.1f, 0.1f, 1.f };
	selectedLight = -1;
	totalTime = 0.f;
	speed = 3.0f;
	amplitude = 0.5f;
	frequency = 2.0f;
}

void App1::init(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight, Input *in, bool VSYNC, bool FULL_SCREEN)
{

	// Call super/parent init function (required!)
	BaseApplication::init(hinstance, hwnd, screenWidth, screenHeight, in, VSYNC, FULL_SCREEN);

	// Load texture
	textureMgr->loadTexture(L"brick", L"res/brick1.dds");

	manipulationShader = std::make_shared<ManipulationShader>(renderer->getDevice(), hwnd);


	// Create Mesh object and shader object
	plane = make_shared<PlaneMesh>(renderer->getDevice(), renderer->getDeviceContext(), 200);
	sphere = make_shared<SphereMesh>(renderer->getDevice(), renderer->getDeviceContext());

	planeMaterial = make_shared<Material>();

	// Create planes material
	planeMaterial->shader = manipulationShader;
	planeMaterial->specularColour = XMFLOAT4(1.f, 1.f, 1.f, 1.f);
	planeMaterial->specularPower = 32.f;
	planeMaterial->texture = L"brick";

	// Create light

	lights.emplace_back(make_shared<Light>(Light(lightTypes::directional)));
	selectedLight = lights.size() - 1;
	lights[0]->setDiffuseColour(1.0f, 1.0f, 1.0f, 1.0f);
	lights[0]->setDirection(0.7f, -0.7f, 0.0f);

	camera->setPosition(35.f, 20.f, 0.f);
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
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;

	// Clear the scene. (default blue colour)
	renderer->beginScene(0.39f, 0.58f, 0.92f, 1.0f);

	// Generate the view matrix based on the camera's position.
	camera->update();

	// Get the world, view, projection, and ortho matrices from the camera and Direct3D objects.
	worldMatrix = renderer->getWorldMatrix();
	viewMatrix = camera->getViewMatrix();
	projectionMatrix = renderer->getProjectionMatrix();
	

	XMMATRIX translation = XMMatrixTranslation(50.f, 25.f, 50.f);
	XMMATRIX scale = XMMatrixScaling(25.f, 25.f, 25.f);

	worldMatrix *= scale;
	worldMatrix *= translation;

	worldMatrix = renderer->getWorldMatrix();

	totalTime += timer->getTime();

	// Send geometry data, set shader parameters, render object with shader
	plane->sendData(renderer->getDeviceContext());
	
	manipulationShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, textureMgr->getTexture(L"brick"), lights[0].get(), totalTime, speed, amplitude, frequency);
	manipulationShader->render(renderer->getDeviceContext(), plane->getIndexCount());


	// Render GUI
	gui();

	// Swap the buffers
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
	ImGui::Text("TIME: %.5f", totalTime);
	ImGui::Checkbox("Wireframe mode", &wireframeToggle);

	XMFLOAT3 camPos = camera->getPosition();
	std::string outputPos = "X: " + std::to_string(camPos.x) + " Y: " + std::to_string(camPos.y) + " Z: " + std::to_string(camPos.z);
	ImGui::Text(outputPos.c_str());

	ImGui::SliderFloat("Speed", &speed, 0.0f, 10.0f);
	ImGui::SliderFloat("Amplitude", &amplitude, 0.0f, 2.0f);
	ImGui::SliderFloat("Frequency", &frequency, 0.1f, 10.0f);

	XMFLOAT3 direction = ToDegrees(lights[0]->getDirectionEuler());
	if (ImGui::DragFloat3("Light Direction Euler: ", &direction.x, 0.1f)) {
		lights[0]->setDirectionEuler(ToRadians(direction));
	}

	// Render UI
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

/*
	Light Imgui


	if (ImGui::CollapsingHeader("Materials")) {

		if (ImGui::TreeNode("Plane Material")) {
			if (ImGui::TreeNode("Specular Colour: ## 0")) {
				if (ImGui::ColorPicker4("Ambient Light: ", &planeMaterial->specularColour.x)) {}
				ImGui::TreePop();
			}
			if (ImGui::SliderFloat("Specular Power: ## 0", &planeMaterial->specularPower, 1.f, 1000.f)) {}
			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Sphere Material")) {
			if (ImGui::TreeNode("Specular Colour: ## 1")) {
				if (ImGui::ColorPicker4("Ambient Light: ", &sphereMaterial->specularColour.x)) {}
				ImGui::TreePop();
			}
			if (ImGui::SliderFloat("Specular Power: ## 1", &sphereMaterial->specularPower, 1.f, 1000.f)) {}
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


*/