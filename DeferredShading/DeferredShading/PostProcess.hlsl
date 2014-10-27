





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
	normal = (normal - 0.5) * 2;

	diffuse = diffuse * dot(vLightDir[0], normal) * 0.5 * vLightColor[0];

	float4 reflection = normalize(reflect(vLightDir[0], normal));
		float4 viewDir = normalize(vEye);

		float4 specularResult = 0;
		if (diffuse.x > 0)
		{
			specularResult = saturate(dot(viewDir, reflection));
			specularResult = pow(specularResult, 32.0f);
			specularResult *= specular*vLightColor[0];
		}

	float4 ambient = float4(0, 0, 0, 1)*0.3;

		float4 finalColor = saturate(ambient + specularResult + diffuse);
		finalColor.a = 1;

	return finalColor;
}

