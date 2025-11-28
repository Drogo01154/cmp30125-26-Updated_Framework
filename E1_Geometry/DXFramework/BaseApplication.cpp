// BaseApplication.cpp
// Base application functionality for inheritnace.
#include "BaseApplication.h"


BaseApplication::BaseApplication()
{
}

// Release resources.
BaseApplication::~BaseApplication()
{

	if (timer)
	{
		delete timer;
		timer = 0;
	}

	if (renderer)
	{
		delete renderer;
		renderer = 0;
	}
}

// Default application initialisation. Create renderer, camera, timer and imGUI objects.
void BaseApplication::init(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight, Input *in, bool VSYNC, bool FULL_SCREEN)
{
	input = in;
	wnd = hwnd;
	sWidth = screenWidth;
	sHeight = screenHeight;
	// Create the Direct3D renderer.
	renderer = new D3D(screenWidth, screenHeight, VSYNC, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
	if (!renderer)
	{
		MessageBox(hwnd, L"Could not initialize DirectX 11.", L"Error", MB_OK);
		exit(EXIT_FAILURE);
	}
	//shaderMgr = std::make_unique<ShaderManager>();
	shaderMgr = std::make_unique<ShaderManager>();
	textureMgr = std::make_unique<TextureManager>(renderer->getDevice(), renderer->getDeviceContext());
	materialMgr = std::make_unique<MaterialManager>(shaderMgr.get(), textureMgr.get());
	geometryMgr = std::make_unique<GeometryManager>(renderer->getDevice(), renderer->getDeviceContext());
	instanceMgr = std::make_unique<InstanceManager>(shaderMgr.get(), geometryMgr.get(), materialMgr.get(), input, wnd, sWidth, sHeight);
	sceneGraph = std::make_unique<SceneGraph>(shaderMgr.get(), instanceMgr.get(), geometryMgr.get());

	//textureMgr->loadTexture(L"default", L"res/DefaultDiffuse.png");


	/*
	// Create the camera object and set to default position.
	camera = new FPCamera(input, sWidth, sHeight, wnd);
	camera->setPosition(0.0f, 0.0f, -10.0f);
	camera->update();
	*/

	// Create the timer object (for delta time and FPS calculation.
	timer = new Timer();

	// Initialise texture manager

	//Initialise ImGUI
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
	ImGui_ImplWin32_Init(hwnd);
	ImGui_ImplDX11_Init(/*hwnd,*/ renderer->getDevice(), renderer->getDeviceContext());

	wireframeToggle = false;
}

// Default frame processing. Check for escape key to exit, update timer, handle input and start UI.
bool BaseApplication::frame()
{
	if (input->isKeyDown(VK_ESCAPE) == true)
	{
		return false;
	}

	timer->frame();

	handleInput(timer->getTime());

	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	renderer->setWireframeMode(wireframeToggle);

	return true;
}


void BaseApplication::handleInput(float frameTime)
{
	CameraData* camData = instanceMgr->getActiveCamera();
	if (camData->type == CameraTypes::FPCAMERA) {
		if (std::shared_ptr<FPCamera> fp = std::dynamic_pointer_cast<FPCamera>(camData->camera)) {
			fp->move(frameTime);
		}
	}
}
