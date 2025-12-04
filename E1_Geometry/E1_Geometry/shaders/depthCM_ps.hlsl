cbuffer PointLightDataModule : register(b0)
{
    float3 lightPosition;
    float farPlane;
};

struct InputType
{
    float4 position : SV_POSITION;
    float4 worldPos : TEXCOORD0;
};

float main(InputType input) : SV_Depth
{
    float depth = length(input.worldPos.xyz - lightPosition) / farPlane;
    return saturate(depth);
}