#include "lightCommon_ps.hlsl"
#include "heightMapCommon_ps.hlsl"

struct InputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
    float3 worldPosition : TEXCOORD1;
    float3 viewVector : TEXCOORD2;
};


float4 main(InputType input) : SV_TARGET
{
    input.normal = calculateHeightMapNormal(input.tex);
    
    float4 diffuse = float4(input.normal, 1.f);
    float4 emissive = calcEmissive(input.tex);
    
    float4 finalColour = ambientLight * diffuse;
    
    for (int i = 0; i < numberOfLights; i++)
    {
        finalColour += calculateLight(diffuse, i, input.worldPosition, input.normal, input.viewVector);
    }
    
    return finalColour + emissive;
}



