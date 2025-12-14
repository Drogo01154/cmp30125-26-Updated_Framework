// Light pixel shader
// Calculate diffuse lighting for a single spot light (also texturing)

//Utilises B0 and t0 pixel shader buffers

Texture2D diffuseTexture : register(t0);
Texture2D emissiveTexture : register(t1);
SamplerState textureSampler : register(s0);

cbuffer CameraDataBuffer : register(b0)
{
    float4 ambientLight; // 16
    float3 cameraPosition; // 12
    int numberOfLights; // 4
    float2 shadowMapTexelSize;  // 8
    float2 padding;            // 8
};  

#define HAS_DIFFUSE   0x1  // 0001
#define HAS_EMISSIVE  0x2  // 0100

cbuffer MaterialDataBuffer : register(b1)
{
    float4 baseColour;          // 16
    float4 specularColour;      // 16
    float specularPower;        // 4
    float emissiveStrength;     // 4
    int flags;                  // 4
    float padding1;            // 4
};


float4 calcEmissive(float2 pos)
{
    return ((flags & HAS_EMISSIVE) != 0) ? emissiveTexture.Sample(textureSampler, pos) * emissiveStrength : float4(0.f, 0.f, 0.f, 0.f);
}

float4 getDiffuse(float2 pos)
{
    return ((flags & HAS_DIFFUSE) != 0) ? diffuseTexture.Sample(textureSampler, pos) * baseColour : baseColour;
}

struct Light // 80 bytes
{
    float4 lightAttenuation;
    float4 lightDiffuse;
    float3 lightDirection;
    float innerConeOrFarPlane; // cos of inner cone angle or far plane for point lights
    float3 lightPosition;
    float outerCone; // cos of outer cone angle
    int type;
    int mapSliceIndex;
    float constBias;
    float slopeBias;
};

StructuredBuffer<Light> lights : register(t2);

float4 calcSpecular(float3 lightDirection, float3 normal, float3 viewVector)
{
    // blinn-phong specular calculation
    float3 halfway = normalize(lightDirection + viewVector);
    float specularIntensity = pow(max(dot(normal, halfway), 0.0), specularPower);
    return saturate(specularColour * specularIntensity);
}

// Calculate lighting intensity based on direction and normal. Combine with light colour.
float4 calculateLightingIntensity(float3 lightDirection, float3 normal)
{
    return saturate(dot(normal, lightDirection));
}

float4 calculateLight(float4 diffuse, int lightNumber, float3 pixelPosition, float3 normal, float3 viewVector)
{
    int lightType = lights[lightNumber].type;
    
    float4 lightDiffuse = diffuse * lights[lightNumber].lightDiffuse;
    
    float4 returnValue;
    
    if (lightType == 0) // Directional Light
    {
        //Calculate light intensity muiltiplied by diffuse
        float4 intensity = calculateLightingIntensity(-lights[lightNumber].lightDirection, normal);
        //Calculate light specular
        float4 specular = calcSpecular(-lights[lightNumber].lightDirection, normal, viewVector);
        //Return only intensity and specular
        returnValue = (lightDiffuse + specular) * intensity;
    } 
    else
    {
        //Create vector from light to pixel
        float3 pixelToLightVec = lights[lightNumber].lightPosition - pixelPosition;
    
        //Calculate distance from pixel to light
        float distance = length(pixelToLightVec);
        
        //If within light range
        if (distance <= lights[lightNumber].lightAttenuation[3])
        {
            //Normalize pixel to light to unit vector
            pixelToLightVec /= distance;
            
             //Calculate Light's Distance Falloff factor
            float attenuation = 1 / (lights[lightNumber].lightAttenuation[0] + (lights[lightNumber].lightAttenuation[1] * distance) + (lights[lightNumber].lightAttenuation[2] * (distance * distance)));
            
            float4 intensity = calculateLightingIntensity(pixelToLightVec, normal);
            
            float4 specular = calcSpecular(pixelToLightVec, normal, viewVector);
            
            returnValue = (lightDiffuse + specular) * intensity * attenuation;
            if (lightType == 2) //  Spot Light
            {
                float cosTheta = dot(pixelToLightVec, normalize(-lights[lightNumber].lightDirection));
                float spotFactor = saturate((cosTheta - lights[lightNumber].outerCone) / (lights[lightNumber].innerConeOrFarPlane - lights[lightNumber].outerCone));
                returnValue *= spotFactor;
            }
        }
        else
        {
            returnValue = float4(0.f, 0.f, 0.f, 0.f);
        }
    }
    return returnValue;
}