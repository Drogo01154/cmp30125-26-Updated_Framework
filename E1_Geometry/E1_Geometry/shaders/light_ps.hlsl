// Light pixel shader
// Calculate diffuse lighting for a single directional light (also texturing)

//https://www.braynzarsoft.net/viewtutorial/q16390-21-spotlights

Texture2D texture0 : register(t0);
SamplerState sampler0 : register(s0);
cbuffer LightBuffer : register(b0)
{
    float4 lightAttenuation;
    float4 lightAmbient;
    float4 lightDiffuse;
    
    float3 lightDirection;
    float lightInnerCone;
    
    float3 lightPosition;
    float lightOuterCone;
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
    input.normal = normalize(input.normal);
    
    float4 diffuse = texture0.Sample(sampler0, input.tex);
    
    float3 finalColour = float3(0.0f, 0.0f, 0.0f);
    
    //Create vector between light position and pixel position
    float3 lightToPixelVec = lightPosition - input.worldPosition;
    
    //Find the distance between the light pos and pixel pos
    float distance = length(lightToPixelVec);
    
    //Add the ambient light
    float3 finalAmbient = diffuse * lightAmbient;
    
    //If pixel is too far, return pixel colour with just the ambient light
    if(distance > lightAttenuation[3])
    {
        return float4(finalAmbient, diffuse.a);
    }
    
    //Turn lightToPxelVec into a unit length vector describing
    // the pixels direction from the lights position
    lightToPixelVec /= distance;
    
     //Calculate how much light the pixel gets by the angle
    //in which the light strikes the pixels surface
    float howMuchLight = dot(lightToPixelVec, input.normal);

    //If light is striking the front side of the pixel
    if (howMuchLight > 0.0f)
    {
        //Add light to the finalColor of the pixel
        finalColour += diffuse * lightDiffuse;
                    
        //Calculate Light's Distance Falloff factor
        finalColour /= (lightAttenuation[0] + (lightAttenuation[1] * distance)) + (lightAttenuation[2] * (distance * distance));

        float pixelToLightVal = dot(-lightToPixelVec, lightDirection);
        
        if (acos(pixelToLightVal) > lightInnerCone)
        {
            finalColour *= pow(max(pixelToLightVal, 0.0f), lightOuterCone);
        }
        
        //Calculate falloff from center to edge of pointlight cone
           
    }
    
    //make sure the values are between 1 and 0, and add the ambient
    finalColour = saturate(finalColour + finalAmbient);
    
    //Return Final Color
    return float4(finalColour, diffuse.a);
}
/*
cbuffer LightBuffer : register(b0)
{
	float4 diffuseColour;
	float3 lightDirection;
	float padding;
};

struct InputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
};


// Calculate lighting intensity based on direction and normal. Combine with light colour.
float4 calculateLighting(float3 lightDirection, float3 normal, float4 diffuse)
{
	float intensity = saturate(dot(normal, lightDirection));
	float4 colour = saturate(diffuse * intensity);
	return colour;
}

float4 main(InputType input) : SV_TARGET
{
	float4 textureColour;
	float4 lightColour;

	// Sample the texture. Calculate light intensity and colour, return light*texture for final pixel colour.
	textureColour = texture0.Sample(sampler0, input.tex);
	lightColour = calculateLighting(-lightDirection, input.normal, diffuseColour);
	
    float4 colour = { input.normal, 1 };
}
*/	

/*
cbuffer LightBuffer : register(b0)
{
    float4 ambientColour;
    float4 diffuseColour;
    float3 lightPosition;
    float padding;
}

struct InputType
{
    float4 position : SV_Position;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
    float3 worldPosition : TEXCOORD1;
};

// Calculate lighting intensity based on direction and normal. Combined with light colour.

float4 calculateLighting(float3 lightDirection, float3 normal, float4 diffuse)
{
    float4 intensity = saturate(dot(normal, lightDirection));
    float4 colour = saturate(diffuse * intensity);
    return colour;
}

float4 main(InputType input) : SV_Target
{
    float4 textureColour;
    float4 lightColour;
	
	// Sample the texture. Calculate light intensity and colour, return light * texture for final pixel colour.
    textureColour = texture0.Sample(sampler0, input.tex);
	
    float3 lightVector = normalize(lightPosition - input.worldPosition);
    lightColour = ambientColour + calculateLighting(lightVector, input.normal, diffuseColour);
	
    return lightColour * textureColour;
}
*/


