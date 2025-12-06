// Texture pixel/fragment shader
// Basic fragment shader for rendering textured geometry

// Texture and sampler registers
Texture2D hdrTexture : register(t0);
SamplerState Sampler0 : register(s0);

struct InputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
};

cbuffer BloomBuffer : register(b0)
{
    float threshold;
    float intensity;
};

//Based on https://learnopengl.com/Advanced-Lighting/Bloom
float4 main(InputType input) : SV_TARGET
{
	// Sample the pixel color from the texture using the sampler at this texture coordinate location.
    float4 textureColour = hdrTexture.Sample(Sampler0, input.tex);
    //Calculate brightness of pixel
    float brightness = dot(textureColour.rgb, float3(0.2126, 0.7152, 0.0722));
    if (brightness > threshold)
        return float4(textureColour.rgb, 1.0);
    else
        return float4(0.0, 0.0, 0.0, 1.0);
}