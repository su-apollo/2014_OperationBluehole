


//--------------------------------------------------------------------------------------
// Input / Output structures
//--------------------------------------------------------------------------------------
struct PS_INPUT
{
	float4 Pos : SV_POSITION;
	float2 Tex : TEXCOORD0;
};


//--------------------------------------------------------------------------------------
// Textures and Samplers
//--------------------------------------------------------------------------------------

Texture2D txSDO : register(t0);
SamplerState samLinear : register(s0);


float4 main(PS_INPUT Input) : SV_TARGET
{
	float4 finalColor = txSDO.Sample(samLinear, Input.Tex);
	return finalColor;
}