#include "kernel_ps.hlsl"

float4 EdgeDetection(float2 pos, float kernelWeights[9])
{
    static const float kernelWeights[9] =
    {
        0.f, -1.f, 0.f,
        -1.f, 4.f, -1.f,
        0.f, -1.f, 0.f
    };
    return Convolution3x3(pos, kernelWeights);
}