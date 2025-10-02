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
	std::shared_ptr<LightShader> shader;
	std::shared_ptr<Light> light;
	std::shared_ptr<PlaneMesh> plane;

	std::shared_ptr<Material> planeMaterial;
	XMFLOAT3 lightPosition = { 35.0f, 30.0f, 25.0f };
	XMFLOAT3 lightDirection = { 0.0f, -1.f, 0.0f };
	XMFLOAT4 lightAttenuation = { 0.05f, 0.01f, 0.001f, 100.f };
	float lightInnerCone = XMConvertToRadians(20.f);
	float lightOuterCone = XMConvertToRadians(40.f);
	XMFLOAT4 ambientLight = { 0.1f, 0.1f, 0.1f, 1.f };
	XMFLOAT4 lightDiffuseColour = { 1.0f, 1.0f, 1.0f, 1.f };
};

#endif