#include "kernel_ps.hlsl"

struct InputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
};


float4 main(InputType input) : SV_TARGET
{
    static const float kernelWeights[9] =
    {
        0.f, -1.f, 0.f,
        -1.f, 4.f, -1.f,
        0.f, -1.f, 0.f
    };
    return Convolution3x3(input.tex, kernelWeights);
}