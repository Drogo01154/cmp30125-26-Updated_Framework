// Application.h
#ifndef _APP1_H
#define _APP1_H

// Includes
#include "../DXFramework/DXF.h"
#include "TexturedLightShader.h"

class App1 : public BaseApplication
{
public:

	App1();
	~App1();
	void init(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight, Input* in, bool VSYNC, bool FULL_SCREEN);

	void initShadows(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight);
	void initBasicLights(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight);

	bool frame();

protected:
	bool render();
	void gui();

private:
};

#endif