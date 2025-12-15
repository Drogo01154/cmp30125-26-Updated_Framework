#include "kernel_ps.hlsl"

float4 Gaussean5x5(float2 pos, float kernelWeights[25])
{
    static const float kernelWeights[25] =
    {
        1.f / 256.f, 4.f / 256.f, 6.f / 256.f, 4.f / 256.f, 1.f / 256.f,
        4.f / 256.f, 16.f / 256.f, 24.f / 256.f, 16.f / 256.f, 4.f / 256.f,
        6.f / 256.f, 24.f / 256.f, 36.f / 256.f, 24.f / 256.f, 6.f / 256.f,
        4.f / 256.f, 16.f / 256.f, 24.f / 256.f, 16.f / 256.f, 4.f / 256.f,
        1.f / 256.f, 4.f / 256.f, 6.f / 256.f, 4.f / 256.f, 1.f / 256.f
    };
    return Convolution5x5(pos, kernelWeights);
}