// Texture pixel/fragment shader
// Basic fragment shader for rendering textured geometry

// Texture and sampler registers
Texture2D thresholdTexture : register(t0);
SamplerState Sampler0 : register(s0);

struct InputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
};

cbuffer BlurBuffer : register(b0)
{
    bool horizontal;
    float2 texelSize;
    float padding;
};

//Based on https://learnopengl.com/Advanced-Lighting/Bloom
float4 main(InputType input) : SV_TARGET
{
    static const float weights[5] = { 0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216 };

    float3 result = thresholdTexture.Sample(Sampler0, input.tex).rgb * weights[0];
    
    if (horizontal)
    {
        for (int i = 1; i < 5; ++i)
        {
            result += thresholdTexture.Sample(Sampler0, input.tex + float2(texelSize.x * i, 0.0f)).rgb * weights[i];
            result += thresholdTexture.Sample(Sampler0, input.tex - float2(texelSize.x * i, 0.0f)).rgb * weights[i];
        }
    }
    else
    {
        for (int i = 1; i < 5; ++i)
        {
            result += thresholdTexture.Sample(Sampler0, input.tex + float2(0.0f, texelSize.y * i)).rgb * weights[i];
            result += thresholdTexture.Sample(Sampler0, input.tex - float2(0.0f, texelSize.y * i)).rgb * weights[i];
        }
    }
    return float4(result, 1.f);
}