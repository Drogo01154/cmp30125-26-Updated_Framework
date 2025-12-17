    #include"lightCommon_ps.hlsl"
StructuredBuffer<float4x4> lightViewProjs : register(t3); // lightProjectionMatrix * lightViewMatrix on the CPU
Texture2DArray shadowMaps : register(t4);
TextureCubeArray cubeMaps : register(t5);
SamplerComparisonState ShadowMapSampler : register(s1);
SamplerComparisonState CubeMapSampler : register(s2);

// Is the gemoetry in our shadow map
bool hasDepthData(float2 uv)
{
    if (uv.x < 0.f || uv.x > 1.f || uv.y < 0.f || uv.y > 1.f)
    {
        return false;
    }
    return true;
}


static const float kernelWeights[25] =
{
    1.f / 256.f, 4.f / 256.f, 6.f / 256.f, 4.f / 256.f, 1.f / 256.f,
    4.f / 256.f, 16.f / 256.f, 24.f / 256.f, 16.f / 256.f, 4.f / 256.f,
    6.f / 256.f, 24.f / 256.f, 36.f / 256.f, 24.f / 256.f, 6.f / 256.f,
    4.f / 256.f, 16.f / 256.f, 24.f / 256.f, 16.f / 256.f, 4.f / 256.f,
    1.f / 256.f, 4.f / 256.f, 6.f / 256.f, 4.f / 256.f, 1.f / 256.f
};

//Calculates difference in direction along X/Y offset in texels
float3 calcCubeOffsetDirection(float3 direction, float2 offset)
{
    float3 up = (abs(direction.y) > 0.999) ?
    float3(1, 0, 0) :
    float3(0, 1, 0);
    
    float3 right = normalize(cross(up, direction));
    up = normalize(cross(direction, right));
    float texelAngle = radians(90.f) * shadowMapTexelSize.x;
    
    return normalize(direction +
    (right * offset.x * texelAngle) +
    (up * offset.y * texelAngle));
}

//Applies gaussean blur to non point shadow 
float Gaussean5X5Normal(float2 pos, int mapIndex, float depthVal)
{
    float shadow = 0;
    [unroll]
    for (int y = -2; y < 3; y++)
    {
        [unroll]
        for (int x = -2; x < 3; x++)
        {
            //Calculates kernel index
            int index = (y + 2) * 5 + (x + 2);
            //Calculates UV offset
            float2 offset = { x * shadowMapTexelSize.x, y * shadowMapTexelSize.y };
            //Accumulates weighted sample
            shadow += shadowMaps.SampleCmpLevelZero(ShadowMapSampler, float3(pos + offset, mapIndex), depthVal).r * kernelWeights[index];
        }
    }
    //Clamps return value 0-1
    return saturate(shadow);
}

//Applies gaussean blur to point shadow
float Gaussean5X5Point(float3 lightDirection, int mapIndex, float depthVal)
{
    float shadow = 0;
    [unroll]
    for (int y = -2; y < 3; y++)
    {
        [unroll]
        for (int x = -2; x < 3; x++)
        {
            int index = (y + 2) * 5 + (x + 2);
            float3 sampleDir = calcCubeOffsetDirection(lightDirection, float2(x, y));
            shadow += cubeMaps.SampleCmpLevelZero(CubeMapSampler, float4(sampleDir, mapIndex), depthVal) * kernelWeights[index];
        }
    }
    //Clamps return value 0-1
    return saturate(shadow);
}
/*
   Converts lights view position to projected coodinates
*/
float2 getProjectiveCoords(float4 lightViewPosition)
{
    // Calculate the projected texture coordinates.
    float2 projTex = lightViewPosition.xy / lightViewPosition.w;
    projTex *= float2(0.5, -0.5);
    projTex += float2(0.5f, 0.5f);
    return projTex;
}
//Calculates shadow factor
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
                 
        shadow = Gaussean5X5Point(lightDirection, mapIndex, fragmentDistance - bias);
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
        
        float depth;
        if (lightType == 0)
        {
            depth = lightViewPos.z;
        }
        else
        {
            depth = lightViewPos.z / lightViewPos.w;
        }
        // Calculate the projected texture coordinates.
        float2 pTexCoord = getProjectiveCoords(lightViewPos);
        
        if (hasDepthData(pTexCoord))
        {   
            shadow = Gaussean5X5Normal(pTexCoord, mapIndex, depth - bias);
        }    
    }
    return shadow;
}