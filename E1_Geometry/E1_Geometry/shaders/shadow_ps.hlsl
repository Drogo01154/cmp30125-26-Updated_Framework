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
    float shadowMapBias = 0.005f;
  
    float4 diffuse = shaderTexture.Sample(diffuseSampler, input.tex);
    float4 colour = ambientLight * diffuse;
    
    int lightViewIndex = 0;
        
    for (int i = 0; i < numberOfLights; i++)
    {
        if (!IsFragmentInShadow(lightViewIndex, i, shadowMapBias, input.worldPos))
        {
            colour += calculateLight(diffuse, i, input.worldPos, input.normal, input.viewVector);
        }
    }
    return saturate(colour);
}