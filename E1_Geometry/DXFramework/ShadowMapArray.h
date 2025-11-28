#pragma once


#ifndef _SHADOW_MAP_ARRAY_H_
#define _SHADOW_MAP_ARRAY_H_

#include "d3d.h"
#include <wrl/client.h>
#include <optional>
using Microsoft::WRL::ComPtr;
using namespace DirectX;

class ShadowMapArray
{
public:
	ShadowMapArray(ID3D11Device* device, int mWidth, int mHeight, int size);
	~ShadowMapArray();

	void resize(
		ID3D11Device* device, 
		std::optional<int> size = std::nullopt, 
		std::optional<int> mWidth = std::nullopt, 
		std::optional<int> mHeight = std::nullopt, 
		bool first = false);

	void BindDsvAndSetNullRenderTarget(ID3D11DeviceContext* dc, int slice);

	inline ID3D11ShaderResourceView* getDepthMapArraySRV() { return mDepthMapArraySRV.Get(); };
private:
	std::vector<ComPtr<ID3D11DepthStencilView>> mDepthMapArrayDSV;
	ComPtr<ID3D11ShaderResourceView> mDepthMapArraySRV;
	D3D11_VIEWPORT viewport;
	ComPtr<ID3D11RenderTargetView> renderTarget;
	ComPtr<ID3D11Texture2D> depthMapArray;
	int shadowMapWidth;
	int shadowMapHeight;
	int size;
};

#endif

