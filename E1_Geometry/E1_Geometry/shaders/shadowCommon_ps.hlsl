#include"lightCommon_ps.hlsl"
StructuredBuffer<float4x4> lightViewProjs : register(t1); // lightProjectionMatrix * lightViewMatrix on the CPU
Texture2DArray shadowMaps : register(t2);
TextureCubeArray cubeMaps : register(t3);
Texture2D shaderTexture : register(t4);
SamplerState ShadowMapSampler : register(s0);
SamplerState CubeMapSampler : register(s1);
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

//Think this won't work for cube maps, might need to change
bool isInShadow(float depthValue, float4 lightViewPosition, float bias)
{
    // Calculate the depth from the light.
    float lightDepthValue = lightViewPosition.z / lightViewPosition.w;
    lightDepthValue -= bias;

	// Compare the depth of the shadow map value and the depth of the light to determine whether to shadow or to light this pixel.
    if (lightDepthValue < depthValue)
    {
        return false;
    }
    return true;
}

bool IsFragmentInShadow(inout float lightViewIndex, int lightID, float shadowMapBias, float3 worldPos)
{
    bool returnValue = true;
        
    int lightType = lights[lightID].type;
    if (lightType == 1) // Point Light
    {
        int cubeMapIndex = lights[lightID].mapSliceIndex;
        for (int z = 0; z < 6; z++)
        { //Get distance from light -> fragment
            float fragmentDistance = length(worldPos - lights[lightID].lightPosition);
                
            //Calculate direction of fragments world position -> light
            float3 direction = normalize(lights[lightID].lightPosition - worldPos);
            //Sample depth value of cubemap - Radial distance from point light -> nearest fragment
            float depthValue = cubeMaps.Sample(CubeMapSampler, float4(direction, cubeMapIndex)).r;
            //if fragment closer than one in map object lit
            if (fragmentDistance <= (depthValue + shadowMapBias))
            {   
                lightViewIndex += (6 - z);
                returnValue = false;
                break;
            }
            cubeMapIndex++;
            lightViewIndex++;
        }

        
    }
    else
    {
        int shadowMapIndex = lights[lightID].mapSliceIndex;
        float4x4 lightViewProj = lightViewProjs[lightViewIndex];
            //Cacluate the lights view position
        float4 lightViewPos = mul(float4(worldPos, 1), lightViewProj);
            // Calculate the projected texture coordinates.
        float2 pTexCoord = getProjectiveCoords(lightViewPos);
            
        // Shadow test. Is or isn't in shadow
        if (hasDepthData(pTexCoord))
        {
            // Sample the shadow map (get depth of geometry)
            float depthValue = shadowMaps.Sample(ShadowMapSampler, float3(pTexCoord, shadowMapIndex)).r;
            returnValue = isInShadow(depthValue, lightViewPos, shadowMapBias);
        }
            
        lightViewIndex++;
    }
    return returnValue;

}