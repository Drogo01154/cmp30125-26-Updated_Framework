// Application.h
#ifndef _APP1_H
#define _APP1_H

// Includes
#include "../DXFramework/DXF.h"
#include "ColourShader.h"
#include "ColourShaderGradient.h"
#include "ColourTriangle.h"
#include "ColourQuad.h"

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
	ColourShader* colourShader;
	ColourGradientShader* colourGradientShader;
	ColourTriangle* triangleMesh;
	ColourQuad* quadMesh;
};

#endif