Texture2D heightMapTexture : register(t6);
SamplerState heightSampler : register(S3);

cbuffer mapData : register(b2)
{
    float2 offset;              // 8  Value to move one texel in UV space
    float heightMultiplier;     // 4
    float heightPadding;        // 4
    float2 worldStep;           // 8 wolrd units per texel
    float UVScale;              // 4
    float heightMapPadding;     // 4
}

float getHeight(float2 coord)
{
    return heightMapTexture.Sample(heightSampler, coord) * heightMultiplier;
}

float3 calculateHeightMapNormal(float2 tex)
{
    
    float heightN = getHeight(float2(tex.x, tex.y + offset.y));
    float heightS = getHeight(float2(tex.x, tex.y - offset.y));
    float heightE = getHeight(float2(tex.x + offset.x, tex.y));
    float heightW = getHeight(float2(tex.x - offset.x, tex.y));
    
    float3 tangent = normalize(float3(2.f * worldStep.x, heightE - heightW, 0.f));
    float3 bitangent = normalize(float3(0.f, heightN - heightS, 2.f * worldStep.y));
    
    return cross(bitangent, tangent);
}