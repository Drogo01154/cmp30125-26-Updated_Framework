#include "shadowCommon_ps.hlsl"
#include "heightMapCommon_ps.hlsl"


struct InputType
{
    float4 position : SV_POSITION;
    float3 worldPos : TEXCOORD0;
    float3 viewVector : TEXCOORD1;
    float2 tex : TEXCOORD2;
    float3 normal : TEXCOORD3;
    float3 vertexNormal : TEXCOORD4;
};

float4 main(InputType input) : SV_TARGET
{ 
    input.normal = calculateHeightMapNormal(input.worldPos);
    
    float4 diffuse = getDiffuse(input.tex * UVScale);
    float4 finalColour = ambientLight * diffuse;
    float4 emissive = calcEmissive(input.tex);
        
    for (int i = 0; i < numberOfLights; i++)
    {
        float shadowScalar = calculateShadow(i, input.worldPos, input.vertexNormal);
        float4 lightColour = calculateLight(diffuse, i, input.worldPos, input.normal, input.viewVector);
        finalColour = float4(shadowScalar, shadowScalar, shadowScalar, 1);
        //finalColour += shadowScalar * lightColour;

    }
    return finalColour;
    //return finalColour + emissive;
}