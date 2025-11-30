cbuffer PointLightDataModule : register(b0)
{
    float3 lightPosition;
    float farPlane;
};

struct InputType
{
    float4 worldPos : TEXCOORD0;
};

float4 main(InputType input) : SV_TARGET
{
    float depthValue = length(input.worldPos.xyz - lightPosition) / farPlane;
    return float4(depthValue, depthValue, depthValue, 1.0);
}