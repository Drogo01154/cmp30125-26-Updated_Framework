// Application.h
#ifndef _APP1_H
#define _APP1_H

// Includes
#include "../DXFramework/DXF.h"
#include "TexturedLightShader.h"
#include "TextureShader.h"
#include "MiniMapTextureShader.h"

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
	void thirdPass();
	void finalPass();
	void gui();

private:

	//Shaders
	/*
	std::shared_ptr<TexturedLightShader> texturedLightShader;
	std::shared_ptr<TextureShader> textureShader;
	std::shared_ptr<MiniMapTextureShader> miniMapTextureShader;
	*/
	uint32_t selectedLight;

	Transform m_transform;
};

#endif