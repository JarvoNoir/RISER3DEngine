struct PS_INPUT
{
	float4 inPos : SV_POSITION;
	float3 inColour : COLOR;
};


float4 main(PS_INPUT input) : SV_TARGET
{
	return float4(input.inColour, 1.0f);
}