// Texture pixel/fragment shader
// Basic fragment shader for rendering textured geometry

// Texture and sampler registers
Texture2D texture0 : register(t0);
SamplerState Sampler0 : register(s0);

struct InputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
};


cbuffer greyScaleBuffer : register(b0) // 48 bytes
{
    float3 greyScaleValues;
    float padding;
}


float4 main(InputType input) : SV_TARGET
{
	// Sample the pixel color from the texture using the sampler at this texture coordinate location.
    float4 textureColor = texture0.Sample(Sampler0, input.tex);

    //Compute grescale luminance
    float grey = dot(textureColor.rgb, greyScaleValues);
    
    return float4(grey, grey, grey, textureColor.a);
}