

//--------------------------------------------------------------------------------------
// Globals
//--------------------------------------------------------------------------------------
cbuffer ConstantBuffer : register(b0)
{
	float4 vEye;
	float4 vLightPos[2];
	float4 vLightColor[2];
	float  fLightRange[2];
};

//--------------------------------------------------------------------------------------
// Textures and Samplers
//--------------------------------------------------------------------------------------

//배열로 관리할 수 있을 것 같은데
Texture2D txNormal : register(t0);
Texture2D txDiffuse : register(t1);
Texture2D txSpecular : register(t2);
Texture2D txDepth : register(t3);
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
	float4 diffuse = txDiffuse.Sample(samLinear, Input.Tex);
	float4 specular = txSpecular.Sample(samLinear, Input.Tex);
	float4 depth = txDepth.Sample(samLinear, Input.Tex);
	float4 ambient = float4(0, 0, 0, 1)*0.3;

	normal = (normal - 0.5) * 2;
	diffuse = diffuse * dot(vLightDir[0], normal) * 0.5 * vLightColor[0];
	
	float4 finalColor = 0;
	finalColor = saturate(ambient + specular + diffuse);

	//for depth
	float n = 1.0f;
	float f = 100.0f;
	float z = (2.0 * n) / (f + n - depth.x * (f - n));

	//finalColor = float4(z, z, z, 1);

	return finalColor;
}

