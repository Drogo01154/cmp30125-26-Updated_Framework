// Light pixel shader
// Calculate diffuse lighting for a single spot light (also texturing)

Texture2D texture0 : register(t0);
SamplerState sampler0 : register(s0);

cbuffer WorldDataBuffer : register(b0)
{
    float4 ambientLight;    // 16
    float4 specularColour;  // 16
    float specularPower;    // 4
    int numberOfLights;     // 4
    float2 padding;         // 8
}

struct Light
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

StructuredBuffer<Light> lights : register(t1);

struct InputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
    float3 worldPosition : TEXCOORD1;
    float3 viewVector : TEXCOORD2;
};

float4 calcSpecular(float3 lightDirection, float3 normal, float3 viewVector, float4
specularColour, float specularPower)
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

float4 calculateLight(int lightNumber, float3 pixelPosition, float3 normal, float3 viewVector)
{
    int lightType = lights[lightNumber].type;
    
    float4 returnValue;
    
    if (lightType == 0) // Directional Light
    {
        //Calculate light intensity muiltiplied by diffuse
        float4 intensity = calculateLightingIntensity(lights[lightNumber].lightDirection, normal, lights[lightNumber].lightDiffuse);
        //Calculate light specular
        float4 specular = calcSpecular(lights[lightNumber].lightDirection, normal, viewVector, specularColour, specularPower);
        //
        returnValue = intensity
        + specular;
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
            
            float4 intensity = calculateLightingIntensity(pixelToLightVec, normal, lights[lightNumber].lightDiffuse);
            
            float4 specular = calcSpecular(pixelToLightVec, normal, viewVector, specularColour, specularPower);
            
            if (lightType == 1) // Point Light
            {
                returnValue = (intensity + specular) * attenuation;

            }
            else if (lightType == 2) //  Spot Light
            {
                //Calculate cos of angle between inverted light direction and pixel to light
                float cosTheta = dot(pixelToLightVec, normalize(-lights[lightNumber].lightDirection));
                float spotFactor = 0.0;
        
                //If inside outer cone
                if (cosTheta > lights[lightNumber].cosLightOuterCone)
                {
                    if (cosTheta >= lights[lightNumber].cosLightInnerCone)
                    {
                        spotFactor = 1.0; // fully inside inner cone
                    }
                    else
                    {
                        //Smooth lerp between inner and outer cone
                        spotFactor = (cosTheta - lights[lightNumber].cosLightOuterCone) / (lights[lightNumber].cosLightInnerCone - lights[lightNumber].cosLightOuterCone);
                    }
                }
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

float4 main(InputType input) : SV_Target
{
    // Get diffuse Colour
    float4 diffuse = texture0.Sample(sampler0, input.tex);
    
    float4 finalColour = ambientLight;
    
    for (int i = 0; i < numberOfLights; i++)
    {
        finalColour += saturate(calculateLight(i, input.worldPosition, input.normal, input.viewVector));
    }
    
    // Multiply by diffuse texture and clamp to 0..1
    return saturate(finalColour * diffuse);
}

/*
float4 main(InputType input) : SV_TARGET
{
    // Get Diffuse Colour
    float4 diffuse = texture0.Sample(sampler0, input.tex);
    
    //Initialise final colour
    float3 finalColour = float3(0.0f, 0.0f, 0.0f);
    
    //Create vector from light to pixel
    float3 pixelToLightVec = lightPosition - input.worldPosition;
    
    //Calculate distance from pixel to light
    float distance = length(pixelToLightVec);
       
    //Normalize pixel to light to unit vector
    pixelToLightVec /= distance;
    
    //If within light range
    if(distance <= lightAttenuation[3])
    {
        //Calculate cos of angle between inverted light direction and pixel to light
        float cosTheta = dot(pixelToLightVec, normalize(-lightDirection)); 
        float spotFactor = 0.0;
        
        //If inside outer cone
        if (cosTheta > cosLightOuterCone)
        {
            if (cosTheta >= cosLightInnerCone)
            {
                spotFactor = 1.0; // fully inside inner cone
            }
            else
            {
                //Smooth lerp between inner and outer cone
                spotFactor = (cosTheta - cosLightOuterCone) / (cosLightInnerCone - cosLightOuterCone);
            }
        }
        //Calculate Lambert factor
        float lambert = saturate(dot(input.normal, pixelToLightVec));
        //Calculate Light's Distance Falloff factor
        float attenuation =  1 / (lightAttenuation[0] + (lightAttenuation[1] * distance) + (lightAttenuation[2] * (distance * distance)));
        
        finalColour = lightAmbient + diffuse * lambert * spotFactor * attenuation;
    }
    else
    {
        //Make only lit by ambient
        finalColour = lightAmbient.rgb;
    }
    
    //Clamp value
    finalColour = saturate(finalColour);
    
    //Return Final Color
    return float4(finalColour, diffuse.a);
}
*/