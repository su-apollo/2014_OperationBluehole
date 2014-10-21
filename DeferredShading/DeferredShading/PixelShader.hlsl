



//--------------------------------------------------------------------------------------
// Globals
//--------------------------------------------------------------------------------------
cbuffer ConstantBuffer : register(b0)
{
	float4 vLightDir[2];
	float4 vLightColor[2];
};

//--------------------------------------------------------------------------------------
// Textures and Samplers
//--------------------------------------------------------------------------------------

Texture2D txDiffuse : register(t0);
SamplerState samLinear : register(s0);

//--------------------------------------------------------------------------------------
// Input / Output structures
//--------------------------------------------------------------------------------------
struct PS_INPUT
{
	float4 Pos : SV_POSITION;
	float3 Norm : TEXCOORD0;
	float2 Tex : TEXCOORD1;
};

//--------------------------------------------------------------------------------------
// Render Target
//--------------------------------------------------------------------------------------
struct GBuffer
{
	float4 normal : SV_TARGET0;
	float4 albedo : SV_TARGET1;
};

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------

GBuffer main(PS_INPUT Input)
{
	GBuffer output;

	output.normal = float4(Input.Norm, 1);
	output.albedo = txDiffuse.Sample(samLinear, Input.Tex);
	
	return output;
}

/*

float4 main(PS_INPUT Input) : SV_TARGET
{
	float4 finalColor = 0;

	//do NdotL lighting for 2 lights
	for (int i = 0; i<2; i++)
	{
		finalColor += saturate(dot((float3)vLightDir[i], Input.Norm) * vLightColor[i]);
	}
	finalColor *= txDiffuse.Sample(samLinear, Input.Tex);
	finalColor.a = 1;
	return finalColor;
}

*/