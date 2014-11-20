
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

Texture2D txSSDO : register(t0);
Texture2D txDiffSpec : register(t1);
SamplerState samLinear : register(s0);


//--------------------------------------------------------------------------------------
// Blur functions
// returns blurred bounced color and blurredOcclusion
//--------------------------------------------------------------------------------------
float4 SsdoBlur(float blurSize, float texelSize, PS_INPUT Input)
{
	float3 blurredBounce = float3(0, 0, 0);
	float	blurredOcclusion = 0.0f;

	float weight[25] = { 0.003765, 0.015019, 0.023792, 0.015019, 0.003765,
		0.015019, 0.059912, 0.094907, 0.059912, 0.015019,
		0.023792, 0.094907, 0.150342, 0.094907, 0.023792,
		0.015019, 0.059912, 0.094907, 0.059912, 0.015019,
		0.003765, 0.015019, 0.023792, 0.015019, 0.003765
	};

	float2 hlim = (float(-blurSize) * 0.5);

	[unroll]
	for (int i = 0; i < blurSize; ++i) {
		for (int j = 0; j < blurSize; ++j) {
			float2 offset = (hlim + float2(float(i), float(j))) * texelSize;
			blurredBounce += (txSSDO.Sample(samLinear, Input.Tex + offset).xyz*weight[blurSize*i + j]);
			blurredOcclusion += (txSSDO.Sample(samLinear, Input.Tex + offset).a*weight[blurSize*i + j]);
		}
	}

	return float4(blurredBounce, blurredOcclusion);
}

float4 main(PS_INPUT Input) : SV_TARGET
{
	float4 ssdo = txSSDO.Sample(samLinear, Input.Tex);
	float3 diffSpec = txDiffSpec.Sample(samLinear, Input.Tex).xyz;
	float3 ambient = float3(1.0f, 1.0f, 1.0f)*0.15;

	float2 texSize;
	txSSDO.GetDimensions(texSize.x, texSize.y);
	float2 texelSize = 1.0 / texSize;
	float blurSize = 5;

	float4 blurredSSDO = SsdoBlur(blurSize, texelSize, Input);

	ambient *= blurredSSDO.a;
	//diffSpec += blurredSSDO.xyz;

	return float4(diffSpec.xyz, 1);
}