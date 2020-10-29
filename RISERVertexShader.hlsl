struct VS_INPUT
{
	float2 inPos : POSITION;
	float3 inColour : COLOR;
};

struct VS_OUTPUT
{
	float4 outPos : SV_POSITION;
	float3 outColour : COLOR;
};

VS_OUTPUT main(VS_INPUT input)
{
	VS_OUTPUT output;
	output.outPos = float4(input.inPos, 0.0f, 1.0f);
	output.outColour = input.inColour;
	return output;
}

