#include "lightCommon_ps.hlsl"
#include "heightMapCommon_ps.hlsl"

Texture2D albedoTexture : register(t2);

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
    input.normal = calculateNormal(input.tex);
    
    // Get diffuse Colour
    float4 diffuse = albedoTexture.Sample(textureSampler, input.tex) * baseColour;
    
    float4 finalColour = ambientLight * diffuse;
    
    for (int i = 0; i < numberOfLights; i++)
    {
        finalColour += calculateLight(diffuse, i, input.worldPosition, input.normal, input.viewVector);
    }
    
    // Multiply by diffuse texture and clamp to 0..1
    return saturate(finalColour);
}



