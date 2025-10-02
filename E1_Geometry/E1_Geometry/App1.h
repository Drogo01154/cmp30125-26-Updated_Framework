// Application.h
#ifndef _APP1_H
#define _APP1_H

// Includes
#include "../DXFramework/DXF.h"
#include "LightShader.h"

class App1 : public BaseApplication
{
public:

	App1();
	~App1();
	void init(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight, Input* in, bool VSYNC, bool FULL_SCREEN);

	bool frame();

protected:
	bool render();
	void gui();

private:
	LightShader* shader;
	PlaneMesh* plane;
	Light* light;
	Material* planeMaterial;
	XMFLOAT3 lightPosition = { 0.0f, 0.0f, 0.0f };
	XMFLOAT4 ambientLight = { 0.1f, 0.1f, 0.1f, 1.f };
	XMFLOAT4 lightDiffuseColour = { 1.0f, 1.0f, 1.0f, 1.f };
};

#endif