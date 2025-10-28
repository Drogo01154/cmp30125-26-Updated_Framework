// Light pixel shader
// Calculate diffuse lighting for a single spot light (also texturing)

//Utilises B0 and t0 pixel shader buffers

cbuffer SceneDataBuffer : register(b0) // 48 bytes
{
    float4 baseColour; // 16
    float4 ambientLight; // 16
    float4 specularColour; // 16
    float specularPower; // 4
    int numberOfLights; // 4
    float2 padding; // 8
}


struct Light // 80 bytes
{
    float4 lightAttenuation;
    float4 lightDiffuse;
    float3 lightDirection;
    float cosLightInnerCone; // cos of inner cone angle
    float3 lightPosition;
    float cosLightOuterCone; // cos of outer cone angle
    int type;
    float3 padding1;
};

StructuredBuffer<Light> lights : register(t0);

float4 calcSpecular(float3 lightDirection, float3 normal, float3 viewVector)
{
    // blinn-phong specular calculation
    float3 halfway = normalize(lightDirection + viewVector);
    float specularIntensity = pow(max(dot(normal, halfway), 0.0), specularPower);
    return saturate(specularColour * specularIntensity);
}


// Calculate lighting intensity based on direction and normal. Combine with light colour.
float4 calculateLightingIntensity(float3 lightDirection, float3 normal, float4 ldiffuse)
{
    float intensity = saturate(dot(normal, lightDirection));
    float4 colour = saturate(ldiffuse * intensity);
    return colour;
}

float4 calculateLight(float4 diffuse, int lightNumber, float3 pixelPosition, float3 normal, float3 viewVector)
{
    int lightType = lights[lightNumber].type;
    
    float4 returnValue;
    
    if (lightType == 0) // Directional Light
    {
        //Calculate light intensity muiltiplied by diffuse
        float4 intensity = calculateLightingIntensity(lights[lightNumber].lightDirection, normal, lights[lightNumber].lightDiffuse);
        //Calculate light specular
        float4 specular = calcSpecular(lights[lightNumber].lightDirection, normal, viewVector);
        //Return only intensity and specular
        returnValue = intensity * diffuse + specular;
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
            
            float4 intensity = calculateLightingIntensity(pixelToLightVec, normal, lights[lightNumber].lightDiffuse) * diffuse;
            
            float4 specular = calcSpecular(pixelToLightVec, normal, viewVector);
            
            if (lightType == 1) // Point Light
            {
                returnValue = (intensity + specular) * attenuation;
            }
            else if (lightType == 2) //  Spot Light
            {
                float cosTheta = dot(pixelToLightVec, normalize(-lights[lightNumber].lightDirection));
                float spotFactor = saturate((cosTheta - lights[lightNumber].cosLightOuterCone) / (lights[lightNumber].cosLightInnerCone - lights[lightNumber].cosLightOuterCone));
                returnValue = (intensity + specular) * attenuation * spotFactor;
            }
        }
        else
        {
            returnValue = float4(0.f, 0.f, 0.f, 0.f);
        }
    }
    return returnValue;
}