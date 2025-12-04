struct InputType
{
    float4 position : SV_POSITION;
    float4 depthPosition : TEXCOORD0;
};

float main(InputType input) : SV_Depth
{
    float depthValue;
	// Get the depth value of the pixel by dividing the Z pixel depth by the homogeneous W coordinate.
    depthValue = input.depthPosition.z / input.depthPosition.w;
    return depthValue;
}