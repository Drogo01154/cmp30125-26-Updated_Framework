// Light pixel shader
// Calculate diffuse lighting for a single directional light (also texturing)

Texture2D texture0 : register(t0);
Texture2D texture1 : register(t1);
SamplerState sampler0 : register(s0);

cbuffer LightBuffer : register(b0)
{
	float4 diffuseColour;
	float3 lightDirection;
	float padding;
};

cbuffer mapData : register(b1)
{
    float2 offset;
    float heightMultiplier;
    float padding1;
}

struct InputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
    float2 mapTex : TEXCOORD1;
	float3 normal : NORMAL;
};

// Calculate lighting intensity based on direction and normal. Combine with light colour.
float4 calculateLighting(float3 lightDirection, float3 normal, float4 diffuse)
{
	float intensity = saturate(dot(normal, lightDirection));
	float4 colour = saturate(diffuse * intensity);
	return colour;
}

float getHeight(float2 coord)
{
    return texture1.Sample(sampler0, coord) * heightMultiplier;
}

float4 main(InputType input) : SV_TARGET
{
	float4 textureColour;
	float4 lightColour;
	
	// Sample the texture. Calculate light intensity and colour, return light*texture for final pixel colour.
	textureColour = texture0.Sample(sampler0, input.tex);

	
    input.position.y = height;
	
	
	
	float x0 = getHeight(input.mapTex - float2(offset.x, 0));
    float x1 = getHeight(input.mapTex + float2(offset.x, 0));
    float z0 = getHeight(input.mapTex - float2(0, offset.y));
    float z1 = getHeight(input.mapTex + float2(0, offset.y));
	
    deltaX = 
	
	float3 xVec = { x1 - x0, height, 0.f };
    float3 zVec = { 0.f, height, z1 - z0 };
	
    input.normal = cross(xVec, zVec);
	
	//lightColour = calculateLighting(-lightDirection, input.normal, diffuseColour);
	
    return textureColour;
}



