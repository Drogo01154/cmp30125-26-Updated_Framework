/**
* \class base shader module
* 
* \brief loads component shader CSOs and sets paramaters
* 
* Base shader module class to be inherited.
*
*/
#ifndef _BASESHADERMODULE_H_
#define _BASESHADERMODULE_H_
#include <d3d11.h>
#include <D3Dcompiler.h>
#include <dxgi.h>
#include <DirectXMath.h>

using namespace std;
using namespace DirectX;

class BaseShaderModule
{
public:
	BaseShaderModule() {};
	BaseShaderModule(ID3D11Device* device, HWND hwnd);
	~BaseShaderModule();
	virtual void initModule() = 0;
	virtual void setResources(ID3D11DeviceContext* deviceContext, size_t startingRegister) = 0;
protected:
	ID3D11Device* renderer;
	HWND hwnd;
};

#endif