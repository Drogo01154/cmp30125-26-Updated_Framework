#include"lightCommon_ps.hlsl"

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
    float4 diffuse = getDiffuse(input.tex);
    float4 emissive = calcEmissive(input.tex);
    
    float4 finalColour = ambientLight * diffuse;
    
    for (int i = 0; i < numberOfLights; i++)
    {
        finalColour += calculateLight(diffuse, i, input.worldPos, input.normal, input.viewVector);
    }
    
    return finalColour + emissive;
}