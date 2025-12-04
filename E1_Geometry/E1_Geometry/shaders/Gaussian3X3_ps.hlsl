#include "kernel_ps.hlsl"

float4 Gaussean3X3(float2 pos, float kernelWeights[9])
{
    static const float kernel[9] =
    {
        1.f / 16.f, 2.f / 16.f, 1.f / 16.f,
        2.f / 16.f, 4.f / 16.f, 2.f / 16.f,
        1.f / 16.f, 2.f / 16.f, 1.f / 16.f
    };
    return Convolution3x3(pos, kernel);
}