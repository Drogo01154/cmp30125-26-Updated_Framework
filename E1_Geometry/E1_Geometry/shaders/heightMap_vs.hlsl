// Light vertex shader
// Standard issue vertex shader, apply matrices, pass info to pixel shader

Texture2D heightMapTexture : register(t0);
SamplerState textureSampler : register(s0);

cbuffer MatrixBuffer : register(b0)
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};

cbuffer CameraBuffer : register(b1)
{
    float3 cameraPosition;
    float padding;
}

cbuffer mapData : register(b2)
{
    float2 offset; // 8
    float heightMultiplier; // 4
    float heightPadding; // 4
    float2 worldStep; // 8
    float2 heightPadding2; // 8
}

struct InputType
{
	float4 position : POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
};

struct OutputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
    float3 worldPosition : TEXCOORD1;
    float3 viewVector : TEXCOORD2;
};

//Shape Deformation
OutputType main(InputType input)
{
    OutputType output;
	
	//Increase Vertex Y position by heighMultiplier.
    input.position.y += heightMapTexture.SampleLevel(textureSampler, input.tex, 0) * heightMultiplier;
	
	// Calculate the position of the vertex against the world, view, and projection matrices.
    output.position = mul(input.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);

	// Store the texture coordinates for the pixel shader.
    output.tex = input.tex;

	// Calculate the normal vector against the world matrix only and normalise.
    output.normal = mul(input.normal, (float3x3) worldMatrix);
    output.normal = normalize(output.normal);
    
    // Calculate the positon of the vertex in the world
    output.worldPosition = mul(input.position, worldMatrix).xyz;
    output.viewVector = cameraPosition.xyz - output.worldPosition.xyz;
    output.viewVector = normalize(output.viewVector);

    return output;
}