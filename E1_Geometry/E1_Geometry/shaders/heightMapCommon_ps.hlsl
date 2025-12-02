// Light pixel shader
// Calculate diffuse lighting for a single directional light (also texturing)

Texture2D heightMapTexture : register(t1);
SamplerState textureSampler : register(s0);

cbuffer mapData : register(b3)
{
    float2 offset;
    float  heightMultiplier;
    int resolution;
}

float getHeight(float2 coord)
{
    return heightMapTexture.Sample(textureSampler, coord) * heightMultiplier;
}

float3 calculateNormal(float2 tex)
{
    float2 worldStep = resolution * offset;
    
    float heightN = getHeight(float2(tex.x, tex.y + offset.y));
    float heightS = getHeight(float2(tex.x, tex.y - offset.y));
    float heightE = getHeight(float2(tex.x + offset.x, tex.y));
    float heightW = getHeight(float2(tex.x - offset.x, tex.y));
    
    float3 tangent = normalize(float3(2.f * worldStep.x, heightE - heightW, 0.f));
    float3 bitangent = normalize(float3(0.f, heightN - heightS, 2.f * worldStep.y));
    
    return cross(bitangent, tangent);
}



