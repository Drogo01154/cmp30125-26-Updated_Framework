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
    float  heightMultiplier;
    int resolution;
}
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

float getHeight(float2 coord)
{
    return texture1.Sample(sampler0, coord) * heightMultiplier;
}

float4 main(InputType input) : SV_TARGET
{
    float2 worldStep = resolution * offset;
    
    float heightN = getHeight(float2(input.tex.x, input.tex.y + offset.y));
    float heightS = getHeight(float2(input.tex.x, input.tex.y - offset.y));
    float heightE = getHeight(float2(input.tex.x + offset.x, input.tex.y));
    float heightW = getHeight(float2(input.tex.x - offset.x, input.tex.y));
    
    
    float3 tangent = normalize(float3(2.f * worldStep.x, heightE - heightW, 0.f));
    float3 bitangent = normalize(float3(0.f, heightN - heightS, 2.f * worldStep.y));
    input.normal = cross(bitangent, tangent);
    //return float4(normalize(input.normal), 1.f);
    
    float4 textureColour;
	float4 lightColour;
	
	// Sample the texture. Calculate light intensity and colour, return light*texture for final pixel colour.
	textureColour = texture0.Sample(sampler0, input.tex);	
    lightColour =  calculateLighting(-lightDirection, input.normal, diffuseColour);
    
    return lightColour * textureColour;

}



