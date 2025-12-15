#include "kernel_ps.hlsl"

float4 BoxBlur(float2 pos, float kernelWeights[9])
{
    static const float kernel[9] =
    {
        1.f / 9.f, 1.f / 9.f, 1.f / 9.f,
        1.f / 9.f, 1.f / 9.f, 1.f / 9.f,
        1.f / 9.f, 1.f / 9.f, 1.f / 9.f
    };
    return Convolution3x3(pos, kernel);
}
