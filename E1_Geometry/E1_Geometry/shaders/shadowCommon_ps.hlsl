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

float calculateShadow(int lightID, float3 worldPos, float3 normal)
{
    int lightType = lights[lightID].type;
    
    float constBias = lights[lightID].constBias;
    float slopeBias = lights[lightID].slopeBias;
    
    int mapIndex = lights[lightID].mapSliceIndex;
    
    float shadow = 1;
    if (lightType == 1) // Point Light
    {
        float3 lightPos = lights[lightID].lightPosition;
        
        float3 lightDirection = normalize(worldPos - lightPos);
    
        // Compute slope-scaled bias
        float ndotl = max(0.0, dot(normal, lightDirection));
        float bias = constBias + slopeBias * (1.0 - ndotl);

        float farPlane = lights[lightID].innerConeOrFarPlane;
        
        // Distance from light to fragment
        float fragmentDistance = length(worldPos - lightPos) / farPlane;
        
        //Uses percentage-close filtering to smooth shadows 
        shadow = cubeMaps.SampleCmpLevelZero(CubeMapSampler, float4(lightDirection, mapIndex), fragmentDistance - bias);
        static const float3 offsets[8] =
        {
            float3(-0.01, -0.01, 0), float3(0, -0.01, 0), float3(0.01, -0.01, 0),
            float3(-0.01, 0, 0), float3(0.01, 0, 0),
            float3(-0.01, 0.01, 0), float3(0, 0.01, 0), float3(0.01, 0.01, 0)
        };
        for (int i = 0; i < 8; i++)
        {
            float3 sampleDir = normalize(lightDirection + offsets[i]);
            shadow += cubeMaps.SampleCmpLevelZero(CubeMapSampler, float4(sampleDir, mapIndex), fragmentDistance - bias);
        }

        shadow /= 9.0f;     
    }
    else    // Spot or directional light
    {
        float3 lightDirection = -lights[lightID].lightDirection;
    
        // Compute slope-scaled bias
        float ndotl = max(0.0, dot(normal, lightDirection));
        float bias = constBias + slopeBias * (1.0 - ndotl);
        
        //Structured buffer of light view proj mapped the same as shadow maps
        float4x4 lightViewProj = lightViewProjs[mapIndex];
        //Cacluate the lights view position
        float4 lightViewPos = mul(float4(worldPos, 1), lightViewProj);
        
        float depth = lightViewPos.z / lightViewPos.w;
        // Calculate the projected texture coordinates.
        float2 pTexCoord = getProjectiveCoords(lightViewPos);
        
        if (hasDepthData(pTexCoord))
        {
            //Uses percentage-close filtering to smooth shadows 
            shadow = shadowMaps.SampleCmpLevelZero(ShadowMapSampler, float3(pTexCoord, mapIndex), depth - bias);

            static const float2 offsets[8] =
            {
                float2(-0.001, -0.001), float2(0, -0.001), float2(0.001, -0.001),
                float2(-0.001, 0), float2(0.001, 0),
                float2(-0.001, 0.001), float2(0, 0.001), float2(0.001, 0.001)
            };
            for (int i = 0; i < 8; i++)
                shadow += shadowMaps.SampleCmpLevelZero(ShadowMapSampler, float3(pTexCoord + offsets[i], mapIndex), depth - bias);

            shadow /= 9.0f;
        }    
    }
    return shadow;
}