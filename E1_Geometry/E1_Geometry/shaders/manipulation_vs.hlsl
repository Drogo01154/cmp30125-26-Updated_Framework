// Light vertex shader
// Standard issue vertex shader, apply matrices, pass info to pixel shader
cbuffer MatrixBuffer : register(b0)
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};

cbuffer TimeBuffer : register(b1)
{
    float time;
    float speed;
    float amplitude;
    float frequency;
}

struct InputType
{
	float4 position : POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
};

struct OutputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
};


/*/Shape Deformation
OutputType main(InputType input)
{
    OutputType output;

	//Offset position based on sine wave
    float offset = amplitude * sin(input.position.x * frequency + time * speed) + (amplitude * cos(input.position.z * frequency + time * speed));
	
    input.position += offset * input.normal;

	
	//Modify normals
    //input.normal = float3(-cos(input.position.x + time), 1, 0);
	
	//Get derivative of sin and cos equatins to get tangent
    float dy_dx = amplitude * frequency * cos(input.position.x * frequency + time * speed);
    float dy_dz = -amplitude * frequency * sin(input.position.z * frequency + time * speed);
	
	//Calculate normal as cross of tangents (if only use single wave calculate normal as perpendicular of tangent)
    input.normal = normalize(cross(float3(0, dy_dz, 1), float3(1, dy_dx, 0)));
	
	// Calculate the position of the vertex against the world, view, and projection matrices.
    output.position = mul(input.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);

	// Store the texture coordinates for the pixel shader.
    output.tex = input.tex;

	// Calculate the normal vector against the world matrix only and normalise.
    output.normal = mul(input.normal, (float3x3) worldMatrix);
    output.normal = normalize(output.normal);

    return output;
}
*/

//Plane Deformation
OutputType main(InputType input)
{
	OutputType output;

	
    input.position.y = sin(input.position.x + time) + cos(input.position.z + time);
	
    float3 partialVecA = { 1, cos(input.position.x + time) * cos(input.position.z + time), 0 };
    float3 partialVecB = { 0, -sin(input.position.z + time) * sin(input.position.x + time), 1 };
	
    input.normal = normalize(-cross(partialVecA, partialVecB));

	//Offset position based on sine wave
    input.position.y = amplitude * sin(input.position.x * frequency + time * speed) + (amplitude * cos(input.position.z * frequency + time * speed));
	
	/*
	float offset = amplitude * sin(input.position.x * frequency + time * speed) + (amplitude * cos(input.position.z * frequency + time * speed));
    input.position += offset * input.normal;
	*/	


	//Get derivative of sin and cos equatins to get tangent
    float dy_dx = amplitude * frequency * cos(input.position.x * frequency + time * speed);
    float dy_dz = -amplitude * frequency * sin(input.position.z * frequency + time * speed);
	
	//Calculate normal as cross of tangents (if only use single wave calculate normal as perpendicular of tangent)
    input.normal = normalize(cross(float3(0, dy_dz, 1), float3(1, dy_dx, 0)));
		

	// Calculate the position of the vertex against the world, view, and projection matrices.
	output.position = mul(input.position, worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);

	// Store the texture coordinates for the pixel shader.
	output.tex = input.tex;

	// Calculate the normal vector against the world matrix only and normalise.
	output.normal = mul(input.normal, (float3x3)worldMatrix);
	output.normal = normalize(output.normal);

	return output;
}


/*
	If y = sin(x + t)

	We can calculate its tangent by getting the derivatie y = sin(
	
	Derivative works out how fast a function is changing at a specific point
	Derivative rules:
		y = 2x, dy/dx = 2;
		y = xsquared = dy/dx = 2x
		y = sin(x), dy/dx = cos(x)
		y = cos(x), dy/dx = -sin(x)
		y = sin(2x + 1), dy/dx = 2cos(2x + 1) == Multiply by derivate of inside 
	
*/