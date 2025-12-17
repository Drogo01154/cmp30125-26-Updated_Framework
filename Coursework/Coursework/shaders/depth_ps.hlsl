cbuffer lightDepthData : register(b0)
{
    float3 lightPosition;
    float farPlane;
    int lightType;
    float3 padding;
};

struct InputType
{
    float4 position : SV_POSITION;
    float3 worldPosition : TEXCOORD0;
    float4 depthPosition : TEXCOORD1;
};

float main(InputType input) : SV_Depth
{
    float depthValue;
    if (lightType == 0) // Directional Light
    {
        depthValue = input.depthPosition.z;
    } else if (lightType == 2) // Spot light
    {   
        depthValue = input.depthPosition.z / input.depthPosition.w;
    }
    else    // Point light
    {
        depthValue = length(input.worldPosition - lightPosition) / farPlane;
    }
    
    return saturate(depthValue);
}