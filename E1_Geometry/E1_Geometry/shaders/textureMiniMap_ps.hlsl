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


cbuffer miniMapBuffer : register(b0) // 48 bytes
{
    float4 PlayerIconColour;
    float3 greyScaleValues;
    float PlayerIconRadius;
    float3 cameraPos; // Camera Screen Space Position
    float padding;
}


float4 main(InputType input) : SV_TARGET
{
    float distanceFromCamera = length(float3(input.position.x, input.position.y, 0.f) - cameraPos);
    if (distanceFromCamera < PlayerIconRadius)
    {
        return PlayerIconColour;
    }
    
	// Sample the pixel color from the texture using the sampler at this texture coordinate location.
    float4 textureColor = texture0.Sample(Sampler0, input.tex);

    //Compute grescale luminance
    float grey = dot(textureColor.rgb, greyScaleValues);
    
    return float4(grey, grey, grey, textureColor.a);
}