#include "CubeMapArray.h"

CubeMapArray::CubeMapArray(ID3D11Device* device, int mWidth, int mHeight, int size) {
	resize(device, size, mWidth, mHeight, true);

	// Setup the viewport for rendering
	viewport.Width = (float)mWidth;
	viewport.Height = (float)mHeight;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;
}

//Function for resizing cube map array buffer based on max number of point lights
void CubeMapArray::resize(ID3D11Device* device, std::optional<int> newSize, std::optional<int> mWidth, std::optional<int> mHeight, bool first) {
	if (!first) {
		mCubeMapArrayDSV.clear();
		mCubeMapArraySRV.Reset();
		depthMapArray.Reset();
	}
	size = newSize == std::nullopt ? size : *newSize;
	shadowMapWidth = mWidth == std::nullopt ? shadowMapWidth : *mWidth;
	shadowMapHeight = mHeight == std::nullopt ? shadowMapHeight : *mHeight;

	//Setup texture description
	D3D11_TEXTURE2D_DESC texDesc;
	texDesc.Width = shadowMapWidth;
	texDesc.Height = shadowMapHeight;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 6 * size;
	texDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	texDesc.CPUAccessFlags = 0;
	texDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;

	device->CreateTexture2D(&texDesc, 0, depthMapArray.GetAddressOf());

	//Resize vector of vector of Depth stencil views to max number of point lights
	mCubeMapArrayDSV.resize(size);

	//For number of point lights
	for (int i = 0; i < size; i++) {
		//For number of faces
		mCubeMapArrayDSV[i].resize(6);
		for (int f = 0; f < 6; f++) {
			
			//Set Depth Stencil View values
			int index = i * 6 + f;
			D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
			dsvDesc.Flags = 0;
			dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
			dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
			dsvDesc.Texture2DArray.MipSlice = 0;
			dsvDesc.Texture2DArray.FirstArraySlice = index;
			dsvDesc.Texture2DArray.ArraySize = 1;
			device->CreateDepthStencilView(depthMapArray.Get(), &dsvDesc, mCubeMapArrayDSV[i][f].GetAddressOf());
		}
	}

	//Create shader resource view to cube map array
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBEARRAY;
	srvDesc.TextureCubeArray.MipLevels = texDesc.MipLevels;
	srvDesc.TextureCubeArray.MostDetailedMip = 0;
	srvDesc.TextureCubeArray.First2DArrayFace = 0;
	srvDesc.TextureCubeArray.NumCubes = size;
	device->CreateShaderResourceView(depthMapArray.Get(), &srvDesc, mCubeMapArraySRV.GetAddressOf());
}

CubeMapArray::~CubeMapArray() {

}

//Function to bind a Depth Stencil View as the current render target
void CubeMapArray::BindDsvAndSetNullRenderTarget(ID3D11DeviceContext* dc, int map, int slice) {
	assert(map < size);
	assert(slice < 6);
	ID3D11RenderTargetView* nullRenderTarger[1] = { nullptr };
	dc->RSSetViewports(1, &viewport);
	dc->OMSetRenderTargets(1, nullRenderTarger, mCubeMapArrayDSV[map][slice].Get());
	dc->ClearDepthStencilView(mCubeMapArrayDSV[map][slice].Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);
}