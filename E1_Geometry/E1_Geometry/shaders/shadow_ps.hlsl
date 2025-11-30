#include"lightCommon_ps.hlsl"
StructuredBuffer<float4x4> lightViewProj : register(t2); // lightProjectionMatrix * lightViewMatrix on the CPU
Texture2DArray shadowMaps : register(t3);
TextureCubeArray cubeMaps : register(t4);
Texture2D shaderTexture : register(t5);
SamplerState ShadowSampler : register(s0);
SamplerState diffuseSampler : register(s1);

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
    float4 colour = ambientLight;
    float4 textureColour = shaderTexture.Sample(diffuseSampler, input.tex);
    
    int lightViewIndex = 0;
    int cubeMapIndex = 0;
    int shadowMapIndex = 0;
        
    for (int i = 0; i < numberOfLights; i++)
    {
        int lightType = lights[i].type;
        
        if (lightType == 1) // Point Light
        {
            for (int z = 0; z < 6; z++)
            {   //Get distance from light -> fragment
                float fragmentDistance = length(input.position.xyz - lights[i].lightPosition);
                
                //Calculate direction of fragments world position -> light
                float3 direction = normalize(input.worldPos - lights[i].lightPosition);
                //Sample depth value of cubemap - Radial distance from point light -> nearest fragment
                float depthValue = cubeMaps.Sample(ShadowSampler, float4(direction, cubeMapIndex)).r;
                //if fragment closer than one in map object lit
                if (fragmentDistance <= (depthValue + shadowMapBias))
                {
                    // is NOT in shadow, therefore light
                    colour += calculateLight(textureColour, i, input.worldPos, input.normal, input.viewVector);
                }
                cubeMapIndex++;
                lightViewIndex++;
            }
            
        }
        else
        {
            //Cacluate the lights view position
            float4 lightViewPos = mul(float4(input.worldPos, 1), lightViewProj[lightViewIndex]);
            // Calculate the projected texture coordinates.
            float2 pTexCoord = getProjectiveCoords(lightViewPos);
            
             // Shadow test. Is or isn't in shadow
            if (hasDepthData(pTexCoord))
            {
                // Sample the shadow map (get depth of geometry)
                float depthValue = shadowMaps.Sample(ShadowSampler, float3(pTexCoord, shadowMapIndex)).r;
                // Has depth map data
                if (!isInShadow(depthValue, lightViewPos, shadowMapBias))
                {
                    // is NOT in shadow, therefore light
                    colour += calculateLight(textureColour, i, input.worldPos, input.normal, input.viewVector);
                }
            }
            
            lightViewIndex++;
            shadowMapIndex++;
        }
    }
}