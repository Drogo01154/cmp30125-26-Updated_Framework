#include "shadowCommon_ps.hlsl"

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
    input.normal = applyNormalMap(input.normal, input.tex);
    float4 diffuse = getDiffuse(input.tex);
    float4 emissive = calcEmissive(input.tex);
    
    float4 finalColour = ambientLight * diffuse;
    
    for (int i = 0; i < numberOfLights; i++)
    {
        float shadowScalar = calculateShadow(i, input.worldPos, input.normal);
        float4 lightColour = calculateLight(diffuse, i, input.worldPos, input.normal, input.viewVector);
        finalColour += shadowScalar * lightColour;
    }
    
    // Multiply by diffuse texture and clamp to 0..1
    return finalColour + emissive;
}