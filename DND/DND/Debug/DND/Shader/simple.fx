cbuffer MatrixWVP
{
	float4x4 wvp : WORLDVIEWPROJECTION;
}

struct VS_INPUT
{
	float4 Pos : POSITION;
	float4 Color : COLOR;
};

struct PS_INPUT
{
	float4 Pos : SV_POSITION;
	float4 Color : COLOR;
};


PS_INPUT VS(VS_INPUT input)
{
	PS_INPUT output = (PS_INPUT)0;
	output.Pos = mul(input.Pos, wvp);
	output.Color = input.Color;
	return output;
}

float4 PS(PS_INPUT input) : SV_Target
{
	return input.Color;
}