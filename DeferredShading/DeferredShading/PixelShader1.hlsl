



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
	float2 Tex : TEXCOORD1;
};

//--------------------------------------------------------------------------------------
// Render Target
//--------------------------------------------------------------------------------------
struct GBuffer
{
	float4 normal : SV_TARGET0;
	float4 diffuse : SV_TARGET1;
	float4 specular : SV_TARGET2;
};

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
GBuffer main(PS_INPUT Input)
{
	GBuffer output;

	// todo : 
	output.normal = 0;
	output.diffuse = txDiffuse.Sample(samLinear, Input.Tex);
	output.specular = 0;

	return output;
}

