// Lab1.cpp
// Lab 1 example, simple coloured triangle mesh
#include "App1.h"

App1::App1()
{
	plane = nullptr;
	shader = nullptr;
	planeMaterial = nullptr;
}

void App1::init(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight, Input *in, bool VSYNC, bool FULL_SCREEN)
{
	// Call super/parent init function (required!)
	BaseApplication::init(hinstance, hwnd, screenWidth, screenHeight, in, VSYNC, FULL_SCREEN);

	// Load texture
	textureMgr->loadTexture(L"brick", L"res/brick1.dds");


	shader = make_shared<LightShader>(LightShader(renderer->getDevice(), textureMgr, hwnd));


	// Create Mesh object and shader object
	plane = make_shared<PlaneMesh>(PlaneMesh(renderer->getDevice(), renderer->getDeviceContext()));
		

	// Create meshes material
	planeMaterial = make_shared<Material>(Material(shader, { 1.f, 1.f, 1.f, 1.f }, 32.f, L"brick"));

	// Initialise light
	light = make_shared<Light>(Light());
	light->setDiffuseColour(lightDiffuseColour.x, lightDiffuseColour.y, lightDiffuseColour.z, lightDiffuseColour.w);
	light->setPosition(lightPosition.x, lightPosition.y, lightPosition.z);
	light->setDirection(lightDirection.x, lightDirection.y, lightDirection.z);
	light->setOuterCone(lightOuterCone);
	light->setInnerCone(lightInnerCone);
	light->setAmbientColour(ambientLight.x, ambientLight.y, ambientLight.z, ambientLight.w);
	light->setAttenuation(lightAttenuation.x, lightAttenuation.y, lightAttenuation.z, lightAttenuation.w);
}


App1::~App1()
{
	// Run base application deconstructor
	BaseApplication::~BaseApplication();
}


bool App1::frame()
{
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

	// Send geometry data, set shader parameters, render object with shader
	plane->sendData(renderer->getDeviceContext());
	shader->setShaderParamaters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, planeMaterial, light);
	shader->render(renderer->getDeviceContext(), plane->getIndexCount());

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
	ImGui::Checkbox("Wireframe mode", &wireframeToggle);

	// Render UI
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

