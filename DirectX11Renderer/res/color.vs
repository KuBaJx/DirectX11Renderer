cbuffer MatrixBuffer
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};

struct SVertexInputType
{
	float4 position : POSITION;
	float4 color : COLOR
};

struct SPixelInputType
{
	float4 position : POSITION;
	float4 color : COLOR;
};

SPixelInputType ColorVertexShader(SVertexInputType input)
{
	SPixelInputType output;

	// Change the pos vector to be 4 units for proper matrix calcs
	input.position.w = 1.0f;

	// Calc the vertex pos against world, view, projection matrices
	output.position = mul(input.position, worldMatrix);
	output.position = mul(input.position, viewMatrix);
	output.position = mul(input.position, projectionMatrix);

	// Store the input color for the pixel shader
	output.color = input.color;

	// Return calculated color to the pixel shader
	return output;
}