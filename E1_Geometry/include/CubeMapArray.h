#pragma once
#ifndef _CUBE_MAP_ARRAY_H_
#define _CUBE_MAP_ARRAY_H_

#include "d3d.h"
#include <wrl/client.h>
#include <optional>
using Microsoft::WRL::ComPtr;
using namespace DirectX;

/*
	Array of cubemaps for shadow maps
	Takes in
		mWidth - width of a cube map slice
		mHeight - height of a cube map slice
		size - number of cube maps
		Allows dynaic resizing
*/
class CubeMapArray
{
public:
	CubeMapArray(ID3D11Device* device, int mWidth, int mHeight, int size);
	~CubeMapArray();

	void resize(
		ID3D11Device* device,
		std::optional<int> size = std::nullopt,
		std::optional<int> mWidth = std::nullopt,
		std::optional<int> mHeight = std::nullopt,
		bool first = false);
	void BindDsvAndSetNullRenderTarget(ID3D11DeviceContext* dc, int map, int slice);

	inline ID3D11ShaderResourceView* getCubeMapArraySRV() { return mCubeMapArraySRV.Get(); };
private:
	std::vector<std::vector<ComPtr<ID3D11DepthStencilView>>> mCubeMapArrayDSV;
	ComPtr<ID3D11ShaderResourceView> mCubeMapArraySRV;
	D3D11_VIEWPORT viewport;
	ComPtr<ID3D11Texture2D> depthMapArray;
	int shadowMapWidth;
	int shadowMapHeight;
	int size;
};

#endif