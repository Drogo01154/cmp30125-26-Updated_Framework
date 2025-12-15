// Texture pixel/fragment shader
// Basic fragment shader for rendering textured geometry

// Texture and sampler registers
Texture2D hdrTexture : register(t0);
Texture2D blurTexture : register(t1);

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
    float exposure;
};

//Based on https://learnopengl.com/Advanced-Lighting/Bloom
float4 main(InputType input) : SV_TARGET
{
    const float gamma = 2.2;
    float3 hdrColour = hdrTexture.Sample(Sampler0, input.tex).rgb;
    float3 bloomColour = blurTexture.Sample(Sampler0, input.tex).rgb;
    
    hdrColour += bloomColour; // additive blending
    // tone mapping
    float3 result = float3(1.f, 1.f, 1.f) - exp(-hdrColour * exposure);
    // also gamma correct while we're at it 
    result = pow(result, float3(1.0f / gamma, 1.0f / gamma, 1.0f / gamma));
    return float4(result, 1.f);
}