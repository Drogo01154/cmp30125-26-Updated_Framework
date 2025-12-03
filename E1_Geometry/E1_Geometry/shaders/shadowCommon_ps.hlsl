#include"lightCommon_ps.hlsl"
StructuredBuffer<float4x4> lightViewProjs : register(t1); // lightProjectionMatrix * lightViewMatrix on the CPU
Texture2DArray shadowMaps : register(t2);
TextureCubeArray cubeMaps : register(t3);
Texture2D shaderTexture : register(t4);
SamplerComparisonState ShadowMapSampler : register(s0);
SamplerComparisonState CubeMapSampler : register(s1);
SamplerState diffuseSampler : register(s2);

// Is the gemoetry in our shadow map
bool hasDepthData(float2 uv)
{
    if (uv.x < 0.f || uv.x > 1.f || uv.y < 0.f || uv.y > 1.f)
    {
        return false;
    }
    return true;
}

/*
   Converts lights 
*/
float2 getProjectiveCoords(float4 lightViewPosition)
{
    // Calculate the projected texture coordinates.
    float2 projTex = lightViewPosition.xy / lightViewPosition.w;
    projTex *= float2(0.5, -0.5);
    projTex += float2(0.5f, 0.5f);
    return projTex;
}

float calculateShadow(inout float lightViewIndex, int lightID, float3 worldPos, float3 normal)
{
    int lightType = lights[lightID].type;
    float3 lightDirection = ((lightType == 0) ? -lights[lightID].lightDirection : lights[lightID].lightDirection);
    
    // Compute slope-scaled bias
    float ndotl = max(0.0, dot(normal, lightDirection));
    float bias = lights[lightID].constBias + lights[lightID].slopeBias * (1.0 - ndotl);
    
    float shadow = 1;
    if (lightType == 1) // Point Light
    {
        // Assuming cubeMaps is a TextureCubeArray
        int cubeMapIndex = lights[lightID].mapSliceIndex;
        float3 lightPos = lights[lightID].lightPosition;

        // Vector from light to fragment
        float3 direction = normalize(worldPos - lightPos);

        // Distance from light to fragment
        float fragmentDistance = length(worldPos - lightPos);
        
        //Uses percentage-close filtering to smooth shadows 
        shadow = cubeMaps.SampleCmpLevelZero(CubeMapSampler, float4(direction, cubeMapIndex), fragmentDistance - bias);
        static const float3 offsets[8] =
        {
            float3(-0.01, -0.01, 0), float3(0, -0.01, 0), float3(0.01, -0.01, 0),
            float3(-0.01, 0, 0), float3(0.01, 0, 0),
            float3(-0.01, 0.01, 0), float3(0, 0.01, 0), float3(0.01, 0.01, 0)
        };
        for (int i = 0; i < 8; i++)
            shadow += cubeMaps.SampleCmpLevelZero(CubeMapSampler, float4(direction + offsets[i], cubeMapIndex), fragmentDistance - bias);

        shadow /= 9.0f;     
    }
    else
    {
        int shadowMapIndex = lights[lightID].mapSliceIndex;
        float4x4 lightViewProj = lightViewProjs[lightViewIndex];
        lightViewIndex++;   // Increment to next matrix as now have enough
        //Cacluate the lights view position
        float4 lightViewPos = mul(float4(worldPos, 1), lightViewProj);
        
        float depth = lightViewPos.z / lightViewPos.w;
        // Calculate the projected texture coordinates.
        float2 pTexCoord = getProjectiveCoords(lightViewPos);
        
        if (hasDepthData(pTexCoord))
        {
            if (hasDepthData(pTexCoord))
            {
                //Uses percentage-close filtering to smooth shadows 
                shadow = shadowMaps.SampleCmpLevelZero(ShadowMapSampler, float3(pTexCoord, shadowMapIndex), depth - bias);

                static const float2 offsets[8] =
                {
                    float2(-0.001, -0.001), float2(0, -0.001), float2(0.001, -0.001),
                    float2(-0.001, 0), float2(0.001, 0),
                    float2(-0.001, 0.001), float2(0, 0.001), float2(0.001, 0.001)
                };
                for (int i = 0; i < 8; i++)
                    shadow += shadowMaps.SampleCmpLevelZero(ShadowMapSampler, float3(pTexCoord + offsets[i], shadowMapIndex), depth - bias);

                shadow /= 9.0f;
            }
        }    
    }
    return shadow;
}