
struct InputType
{
    float4 position : POSITION;
    float2 uv : TEXCOORD;
};


float4 main(InputType input) : SV_TARGET
{
    return float4(input.uv.y, input.uv.y, input.uv.y, 1.f);
}