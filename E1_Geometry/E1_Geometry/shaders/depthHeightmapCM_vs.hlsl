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
    float4 worldPos : TEXCOORD0;
};

OutputType main(InputType input)
{
    OutputType output;
    
    //Increase Vertex Y position by heighMultiplier.
    input.position.y += heightMapTexture.SampleLevel(textureSampler, input.tex, 0) * heightMultiplier;
     // Calculate the position of the vertex against the world, view, and projection matrices.
    output.worldPos = mul(input.position, worldMatrix);
    output.position = mul(output.worldPos, viewMatrix);
    output.position = mul(output.position, projectionMatrix);
    return output;
}