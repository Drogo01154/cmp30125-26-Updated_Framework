#include"lightCommon_ps.hlsl"

// Light pixel shader
// Calculate diffuse lighting for a single spot light (also texturing)

Texture2D texture0 : register(t1);
SamplerState sampler0 : register(s0);

struct InputType
{
    float4 position : SV_POSITION;
    float3 worldPos : TEXCOORD0;
    float3 viewVector : TEXCOORD1;
    float2 tex : TEXCOORD2;
    float3 normal : NORMAL;
};

float4 main(InputType input) : SV_Target
{
    // Get diffuse Colour
    float4 diffuse = texture0.Sample(sampler0, input.tex) * baseColour;
    
    float4 finalColour = ambientLight * diffuse;
    
    for (int i = 0; i < numberOfLights; i++)
    {
        finalColour += calculateLight(diffuse, i, input.worldPos, input.normal, input.viewVector);
    }
    
    // Multiply by diffuse texture and clamp to 0..1
    return finalColour;
}