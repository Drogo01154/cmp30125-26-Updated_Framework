// Light pixel shader
// Calculate diffuse lighting for a single spot light (also texturing)

Texture2D texture0 : register(t0);
SamplerState sampler0 : register(s0);
cbuffer LightBuffer : register(b0)
{
    float4 lightAttenuation;
    float4 lightAmbient;
    float4 lightDiffuse;
    
    float3 lightDirection; 
    float cosLightInnerCone; // cos of inner cone angle
    
    float3 lightPosition;
    float cosLightOuterCone; // cos of outer cone angle
}

struct InputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
    float3 worldPosition : TEXCOORD1;
};

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