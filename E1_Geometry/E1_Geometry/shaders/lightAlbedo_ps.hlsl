#include"lightCommon_ps.hlsl"

// Light pixel shader
// Calculate diffuse lighting for a single spot light (also texturing)

struct InputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
    float3 worldPosition : TEXCOORD1;
    float3 viewVector : TEXCOORD2;
};

float4 main(InputType input) : SV_Target
{
    // Get diffuse Colour
    float4 diffuse = baseColour;
    
    float4 finalColour = ambientLight * diffuse;
    
    for (int i = 0; i < numberOfLights; i++)
    {
        finalColour += calculateLight(diffuse, i, input.worldPosition, input.normal, input.viewVector);
    }
    
    // Multiply by diffuse texture and clamp to 0..1
    return saturate(finalColour);
}