#include"lightCommon_ps.hlsl"
StructuredBuffer<float4x4> lightViewProjs : register(t1); // lightProjectionMatrix * lightViewMatrix on the CPU
Texture2DArray shadowMaps : register(t2);
TextureCubeArray cubeMaps : register(t3);
Texture2D shaderTexture : register(t4);
SamplerComparisonState ShadowMapSampler : register(s0);
SamplerComparisonState CubeMapSampler : register(s1);
SamplerState diffuseSampler : register(s2);
/*
static const float kernelWeights[9] =
{
        1.f / 16.f, 2.f / 16.f, 1.f / 16.f,
        2.f / 16.f, 4.f / 16.f, 2.f / 16.f,
        1.f / 16.f, 2.f / 16.f, 1.f / 16.f
};
*/

static const float kernelWeights[9] =
{
        1.f / 9.f, 1.f / 9.f, 1.f / 9.f,
        1.f / 9.f, 1.f / 9.f, 1.f / 9.f,
        1.f / 9.f, 1.f / 9.f, 1.f / 9.f
};

float3 calcCubeOffsetDirection(float3 direction, float2 offset)
{
    float3 up = (abs(direction.y) < 0.999) ?
    float3(0, 1, 0):
    float3(1, 0, 0);
    
    float3 right = normalize(cross(up, direction));
    up = normalize(cross(direction, right));
    float texelAngle = radians(90.f) * shadowMapTexelSize.x;
    
    return normalize(direction +
    (right * offset.x * texelAngle) +
    (up * offset.y * texelAngle));  
}


float Guassean3X3Normal(float2 pos, int mapIndex, float depthVal)
{
    float shadow = 0;
    [unroll]
    for (int y = -1; y < 2; y++)
    {
        [unroll]
        for (int x = -1; x < 2; x++)
        {
            //Calculate kernel index
            int index = (y + 1) * 3 + x + 1;
            //Calculate sample offset
            float2 offset = { x * shadowMapTexelSize.x, y * shadowMapTexelSize.y };
            //Accumulate weighted sample
            shadow += shadowMaps.SampleCmpLevelZero(ShadowMapSampler, float3(pos + offset, mapIndex), depthVal).r * kernelWeights[index];
        }
    }
    shadow = saturate(shadow);
    return shadow;
}

float Guassean3X3Point(float3 lightDirection, int mapIndex, float depthVal)
{
    float shadow = 0;
    [unroll]
    for (int y = -1; y < 2; y++)
    {
        [unroll]
        for (int x = -1; x < 2; x++)
        {
            float3 sampleDir = calcCubeOffsetDirection(lightDirection, float2(x, y));
            shadow += cubeMaps.SampleCmpLevelZero(CubeMapSampler, float4(sampleDir, mapIndex), depthVal);
        }
    }
    shadow = saturate(shadow);
    return shadow;
}

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
    static const float2 offsets[8] =
    {
        float2(-1, -1), float2(0, -1), float2(1, -1),
        float2(-1, 0), float2(1, 0),
        float2(-1, 1), float2(0, 1), float2(1, 1)
    };
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
                for (int i = 0; i < 8; i++)
        {
            float3 sampleDir = calcCubeOffsetDirection(lightDirection, offsets[i]);
            shadow += cubeMaps.SampleCmpLevelZero(CubeMapSampler, float4(sampleDir, mapIndex), fragmentDistance - bias);
        }

        shadow /= 9.0f;    
        
        //shadow = Guassean3X3Point(lightDirection, mapIndex, fragmentDistance - bias);

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

           

            for (int i = 0; i < 8; i++)
                shadow += shadowMaps.SampleCmpLevelZero(ShadowMapSampler, float3(pTexCoord + offsets[i] * shadowMapTexelSize, mapIndex), depth - bias);
            
            shadow /= 9.0f;
        
            
            //shadow = Guassean3X3(pTexCoord, mapIndex, depth - bias);
           
        }    
    }
    return shadow;
}