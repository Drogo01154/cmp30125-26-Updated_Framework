#include "ColourQuad.h"

// QuadMesh.cpp
// Simple quad mesh for example purposes with texture coordinates and normals.

// Initialise buffers and load textures
ColourQuad::ColourQuad(ID3D11Device* device, ID3D11DeviceContext* deviceContext) {
	initBuffers(device);
}

//Release resources
ColourQuad::~ColourQuad(){
	// Run parent deconstructor
	BaseMesh::~BaseMesh();
}

// Build shape and fill buffers. 
void ColourQuad::initBuffers(ID3D11Device* device) {
	D3D11_SUBRESOURCE_DATA vertexData, indexData;

	vertexCount = 4;
	indexCount = 6;
	VertexType_Texture* vertices = new VertexType_Texture[vertexCount];
	unsigned long* indices = new unsigned long[indexCount];

	// Load the vertex array with data.
	vertices[0].position = XMFLOAT3(-1.0f, 1.0f, 0.0f);  // Top Left
	vertices[0].texture = XMFLOAT2(0.f, 0.f);

	vertices[1].position = XMFLOAT3(-1.0f, -1.0f, 0.0f);  // bottom left.
	vertices[1].texture = XMFLOAT2(0.f, 1.f);

	vertices[2].position = XMFLOAT3(1.0f, -1.0f, 0.0f);  // bottom right.
	vertices[2].texture = XMFLOAT2(1.f, 1.f);

	vertices[3].position = XMFLOAT3(1.0f, 1.0f, 0.0f);  // Top right.
	vertices[3].texture = XMFLOAT2(1.f, 0.f);

	// Load the index array with data.
	indices[0] = 0;  // Top Left.
	indices[1] = 1;  // Bottom left.
	indices[2] = 2;  // Bottom right.
	indices[3] = 0;  // Top Left
	indices[4] = 2;  // Bottom right.
	indices[5] = 3;  // Top Right

	D3D11_BUFFER_DESC vertexBufferDesc = { sizeof(VertexType_Texture) * vertexCount, D3D11_USAGE_DEFAULT, D3D11_BIND_VERTEX_BUFFER, 0, 0, 0 };
	vertexData = { vertices, 0 , 0 };
	device->CreateBuffer(&vertexBufferDesc, &vertexData, &vertexBuffer);

	D3D11_BUFFER_DESC indexBufferDesc = { sizeof(unsigned long) * indexCount, D3D11_USAGE_DEFAULT, D3D11_BIND_INDEX_BUFFER, 0, 0, 0 };
	indexData = { indices, 0, 0 };
	device->CreateBuffer(&indexBufferDesc, &indexData, &indexBuffer);

	// Release the arrays now that the vertex and index buffers have been created and loaded.
	delete[] vertices;
	vertices = 0;
	delete[] indices;
	indices = 0;
}

void ColourQuad::sendData(ID3D11DeviceContext* deviceContext, D3D_PRIMITIVE_TOPOLOGY top)
{
	unsigned int stride;
	unsigned int offset;

	// Set vertex buffer stride and offset.
	stride = sizeof(VertexType_Texture);
	offset = 0;

	deviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	deviceContext->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	deviceContext->IASetPrimitiveTopology(top);
}