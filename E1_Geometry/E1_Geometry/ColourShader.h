// Colour shader.h
// Simple shader example.
#ifndef _COLOURSHADER_H_
#define _COLOURSHADER_H_

#include "DXF.h"

class ColourShader : public BaseShader
{

public:

	ColourShader(ShaderManager* shaderManager, ID3D11Device* device, HWND hwnd);
	~ColourShader();

	void setResources(ID3D11DeviceContext* deviceContext);

	inline void testStuff(int& testInt) {
		testInt = 5;
	}

private:
	void initShader(const wchar_t* vs, const wchar_t* ps);

private:
	ModuleInstance matrixDataModule;
};

#endif