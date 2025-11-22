#pragma once
// Colour shader.h
// Simple shader example.
#ifndef _COLOURGRADIENTSHADER_H_
#define _COLOURGRADIENTSHADER_H_

#include "DXF.h"

class ColourGradientShader : public BaseShader
{

public:

	ColourGradientShader(ID3D11Device* device, HWND hwnd);
	~ColourGradientShader();

	void setResources(ID3D11DeviceContext* deviceContext);

private:
	void initShader(const wchar_t* vs, const wchar_t* ps);

private:
	ModuleInstance matrixDataModule;
};

#endif