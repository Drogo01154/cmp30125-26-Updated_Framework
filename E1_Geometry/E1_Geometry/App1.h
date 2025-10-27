// Application.h
#ifndef _APP1_H
#define _APP1_H

// Includes
#include "../DXFramework/DXF.h"
#include "LightShader.h"
#include "TextureShader.h"

class App1 : public BaseApplication
{
public:

	App1();
	~App1();
	void init(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight, Input* in, bool VSYNC, bool FULL_SCREEN);

	bool frame();

protected:
	bool render();
	void firstPass();
	void finalPass();
	void gui();

private:

	//Shaders
	std::shared_ptr<LightShader> lightShader;
	std::shared_ptr<TextureShader> textureShader;

	//Objects
	std::shared_ptr<CubeMesh> cubeMesh;
	std::shared_ptr<SphereMesh> sphereMesh;
	std::shared_ptr<OrthoMesh> orthoMesh;

	//Lights
	std::vector<std::shared_ptr<Light>> lights;
	
	//Materials
	std::shared_ptr<Material> shapeMaterial;

	//Textures
	std::shared_ptr<RenderTexture> renderTexture;

	//Cameras
	std::shared_ptr<Camera> secondCamera;

	XMFLOAT4 ambientLight;
	uint32_t selectedLight;

	std::string LightTypeStrings[3] =
	{
		"directional",
		"point",
		"spot"
	};
};

#endif