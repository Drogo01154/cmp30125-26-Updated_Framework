#pragma once
// Colour shader.h
// Simple shader example.
#ifndef _COLOURGRADIENTSHADER_H_
#define _COLOURGRADIENTSHADER_H_

#include "../DXFramework/BaseShader.h"

class ColourGradientShader : public BaseShader
{

public:

	ColourGradientShader(ID3D11Device* device, HWND hwnd);
	~ColourGradientShader();

	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& world, const XMMATRIX& view, const XMMATRIX& projection);

private:
	void initShader(const wchar_t* vs, const wchar_t* ps);

private:
	ID3D11Buffer* matrixBuffer;
};

#endif