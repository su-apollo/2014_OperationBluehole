
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
	float weight[25] = { 0.003765, 0.015019, 0.023792, 0.015019, 0.003765,
	0.015019, 0.059912, 0.094907, 0.059912, 0.015019,
	0.023792, 0.094907, 0.150342, 0.094907, 0.023792,
	0.015019, 0.059912, 0.094907, 0.059912, 0.015019,
	0.003765, 0.015019, 0.023792, 0.015019, 0.003765
	};
	float3 diffuseSpecular = txSDO.Sample(samLinear, Input.Tex).xyz;
	float3 ambient = float3(1.0f, 1.0f, 1.0f)*0.15;

	float2 texSize;
	txSDO.GetDimensions(texSize.x, texSize.y);
	float2 texelSize = 1.0 / texSize;
	float result = 0.0;
	float blurSize = 5;

	float2 hlim = (float(-blurSize) * 0.5 );
	for (int i = 0; i < blurSize; ++i) {
		for (int j = 0; j < blurSize; ++j) {
			float2 offset = (hlim + float2(float(i), float(j))) * texelSize;
			result += (txSDO.Sample(samLinear, Input.Tex + offset).a*weight[blurSize*i + j]);
		}
	}

	ambient *= result;
	
	//addBlur
	float4 finalColor = float4(saturate(diffuseSpecular), 1);
	//finalColor = txSDO.Sample(samLinear, Input.Tex).a;
	//finalColor = float4(ambient,1);
	return finalColor;
}