#ifndef _BASESHADERMODULE_H_
#define _BASESHADERMODULE_H_
#include <d3d11.h>
#include <D3Dcompiler.h>
#include <dxgi.h>
#include <DirectXMath.h>

using namespace std;
using namespace DirectX;
/*
	Class Base Shader Module
	- Modules store buffer data for shaders
	- Shaders set their resources from modules stored buffers 
	- Reduces mapping and unmapping of buffers to only when data changes and not every time shader is switched.
*/
class BaseShaderModule
{
public:
	BaseShaderModule() {};
	BaseShaderModule(ID3D11Device* device, HWND hwnd);
	virtual void setResources(D3D11_SHADER_VERSION_TYPE shaderType, ID3D11DeviceContext* deviceContext, size_t startingRegister) = 0;
protected:
	ID3D11Device* renderer;
	HWND hwnd;
};

#endif