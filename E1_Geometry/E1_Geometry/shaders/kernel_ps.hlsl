// Texture and sampler registers
Texture2D texture0 : register(t0);
SamplerState Sampler0 : register(s0);

cbuffer texelBuffer : register(b0) // 16 bytes
{
    float texelWidth; // 4 bytes, 1 / screenWidth
    float texelHeight; // 4 bytes, 1 / screenHeight
    float2 padding; // 8 bytes
}

float4 Convolution5x5(float2 pos, float kernelWeights[25])
{
    float4 colour = 0;
    [unroll]
    for (int y = -2; y < 3; y++)
    {
        [unroll]
        for (int x = -2; x < 3; x++)
        {
            //Calculate kernel index
            int index = (y + 2) * 5 + x + 2;
            //Calculate sample offset
            float2 offset = { x * texelWidth, y * texelHeight };
            //Accumulate weighted sample
            colour += texture0.Sample(Sampler0, pos + offset) * kernelWeights[index];

        }

    }
    colour.rgb = saturate(colour.rgb);
    colour.a = 1;
    return colour;
}

float4 Convolution3x3(float2 pos, float kernelWeights[9])
{
    float4 colour = 0;
    [unroll]
    for (int y = -1; y < 2; y++)
    {
        [unroll]
        for (int x = -1; x < 2; x++)
        {
            //Calculate kernel index
            int index = (y + 1) * 3 + x + 1;
            //Calculate sample offset
            float2 offset = { x * texelWidth, y * texelHeight };
            //Accumulate weighted sample
            colour += texture0.Sample(Sampler0, pos + offset) * kernelWeights[index];
        }
    }
    colour.rgb = saturate(colour.rgb);
    colour.a = 1;
    return colour;
}
