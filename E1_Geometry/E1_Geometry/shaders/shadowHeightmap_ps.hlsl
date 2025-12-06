#include "shadowCommon_ps.hlsl"
#include "heightMapCommon_ps.hlsl"

struct InputType
{
    float4 position : SV_POSITION;
    float3 worldPos : TEXCOORD0;
    float3 viewVector : TEXCOORD1;
    float2 tex : TEXCOORD2;
    float3 normal : NORMAL;
};

float4 main(InputType input) : SV_TARGET
{ 
    input.normal = calculateNormal(input.tex);
    
    float4 diffuse = shaderTexture.Sample(textureSampler, input.tex);
    float4 colour = ambientLight * diffuse;
        
    for (int i = 0; i < numberOfLights; i++)
    {
        float shadowScalar = calculateShadow(i, input.worldPos, input.normal);
        float4 lightColour = calculateLight(diffuse, i, input.worldPos, input.normal, input.viewVector);
        colour += shadowScalar * lightColour;

    }
    return colour;
}