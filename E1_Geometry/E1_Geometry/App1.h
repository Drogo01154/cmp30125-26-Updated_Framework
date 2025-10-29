// Application.h
#ifndef _APP1_H
#define _APP1_H

// Includes
#include "../DXFramework/DXF.h"
#include "LightTextureShader.h"
#include "LightAlbedoShader.h"
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
	void secondPass();
	void finalPass();
	void gui();

private:

	//Shaders
	std::shared_ptr<LightTextureShader> lightTextureShader;
	std::shared_ptr<LightAlbedoShader> lightAlbedoShader;
	std::shared_ptr<TextureShader> textureShader;


	//Objects
	std::shared_ptr<CubeMesh> cubeMesh;
	std::shared_ptr<SphereMesh> sphereMesh;
	std::shared_ptr<OrthoMesh> orthoMeshTL;
	std::shared_ptr<OrthoMesh> otherMeshTR;

	//Lights
	std::vector<std::shared_ptr<Light>> lights;
	
	//Materials
	std::shared_ptr<Material> shapeTextureMaterial;
	std::shared_ptr<Material> shapeAlbedoMaterial;

	//Textures
	std::shared_ptr<RenderTexture> renderTextureTL;
	std::shared_ptr<RenderTexture> renderTextureTR;

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