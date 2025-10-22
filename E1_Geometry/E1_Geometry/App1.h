// Application.h
#ifndef _APP1_H
#define _APP1_H

// Includes
#include "../DXFramework/DXF.h"
#include "LightShader.h"
#include "HeightMapShader.h"

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
	std::shared_ptr<HeightMapShader> heightMapShader;

	std::vector<std::shared_ptr<Light>> lights;

	std::shared_ptr<PlaneMesh> plane;

	std::shared_ptr<Material> planeMaterial;
	XMFLOAT4 ambientLight;
	uint32_t selectedLight;

	float heightMultiplier;

	std::string LightTypeStrings[3] =
	{
		"directional",
		"point",
		"spot"
	};
};

#endif