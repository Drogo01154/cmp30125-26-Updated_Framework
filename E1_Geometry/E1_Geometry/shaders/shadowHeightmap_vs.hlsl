Texture2D heightMapTexture : register(t0);
SamplerState textureSampler : register(s0);

cbuffer MatrixBuffer : register(b0)
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};

cbuffer CameraDataBuffer : register(b1)
{
    float4 ambientLight; // 16
    float3 cameraPosition; // 12
    int numberOfLights; // 4
};

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
    float3 worldPos : TEXCOORD0;
    float3 viewVector : TEXCOORD1;
    float2 tex : TEXCOORD2;
	float3 normal : NORMAL;
};


OutputType main(InputType input)
{
    OutputType output;
    
    //Increase Vertex Y position by heighMultiplier.
    input.position.y += heightMapTexture.SampleLevel(textureSampler, input.tex, 0) * heightMultiplier;

	// Calculate the position of the vertex against the world, view, and projection matrices.
    output.position = mul(input.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);
    
    // Calculate the positon of the vertex in the world
    output.worldPos = mul(input.position, worldMatrix).xyz;
    output.viewVector = cameraPosition.xyz - output.worldPos.xyz;
    output.viewVector = normalize(output.viewVector);


    output.tex = input.tex;
    output.normal = mul(input.normal, (float3x3)worldMatrix);
    output.normal = normalize(output.normal);

	return output;
}