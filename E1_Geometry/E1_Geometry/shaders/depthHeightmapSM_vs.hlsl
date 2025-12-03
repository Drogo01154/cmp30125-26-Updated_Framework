Texture2D heightMapTexture : register(t0);
SamplerState textureSampler : register(s0);

cbuffer MatrixBuffer : register(b0)
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
};

cbuffer mapData : register(b1)
{
    float2 offset;
    float heightMultiplier;
    int resolution;
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
    float4 depthPosition : TEXCOORD0;
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

    // Store the position value in a second input value for depth value calculations.
    output.depthPosition = output.position;
	
    return output;
}