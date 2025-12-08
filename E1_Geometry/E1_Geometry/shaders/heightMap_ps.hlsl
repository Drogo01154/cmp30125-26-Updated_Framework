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
    
    // Get diffuse Colour
    float4 diffuse = getDiffuse(input.tex);
    float4 emissive = calcEmissive(input.tex);
    
    float4 finalColour = ambientLight * diffuse;
    
    for (int i = 0; i < numberOfLights; i++)
    {
        finalColour += calculateLight(diffuse, i, input.worldPosition, input.normal, input.viewVector);
    }
    
    // Multiply by diffuse texture and clamp to 0..1
    return finalColour + emissive;
}



