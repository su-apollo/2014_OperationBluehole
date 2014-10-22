





//--------------------------------------------------------------------------------------
// Globals
//--------------------------------------------------------------------------------------
cbuffer ConstantBuffer : register(b0)
{
	float4 vEye;
	float4 vLightDir[2];
	float4 vLightColor[2];
};

//--------------------------------------------------------------------------------------
// Textures and Samplers
//--------------------------------------------------------------------------------------

Texture2D txNormal : register(t0);
Texture2D txAlbedo : register(t1);
Texture2D txDepth : register(t2);
SamplerState samLinear : register(s0);

//--------------------------------------------------------------------------------------
// Input / Output structures
//--------------------------------------------------------------------------------------
struct PS_INPUT
{
	float4 Pos : SV_POSITION;
	float2 Tex : TEXCOORD0;
};


//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------

float4 main(PS_INPUT Input) : SV_TARGET
{
	float4 normal = txNormal.Sample(samLinear, Input.Tex);
	float4 albedo = txAlbedo.Sample(samLinear, Input.Tex);
	float4 depth = txDepth.Sample(samLinear, Input.Tex);

	float4 finalColor = 0;

	/*
	//do NdotL lighting for 2 lights
	for (int i = 0; i<2; i++)
	{
	finalColor += saturate(dot(vLightDir[i], normal) * vLightColor[i]);
	}
	finalColor *= albedo;
	finalColor.a = 1;
	*/

	// todo : unpack
	//finalColor = normal;
	finalColor = albedo;
	//finalColor = float4(depth.x, depth.x, depth.x, 1);
	finalColor.a = 1;

	return finalColor;
}

