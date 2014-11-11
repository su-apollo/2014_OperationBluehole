

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
	float3 diffuseSpecular = txSDO.Sample(samLinear, Input.Tex).xyz;
	float3 ambient = float3(1.0f, 1.0f, 1.0f)*0.15;

	float2 texSize;
	txSDO.GetDimensions(texSize.x, texSize.y);
	float2 texelSize = 1.0 / texSize;
	float result = 0.0;
	float blurSize = 4;

	float2 hlim = (float(-blurSize) * 0.5 + 0.5);
	for (int i = 0; i < blurSize; ++i) {
		for (int j = 0; j < blurSize; ++j) {
			float2 offset = (hlim + float2(float(i), float(j))) * texelSize;
			result += txSDO.Sample(samLinear, Input.Tex + offset).a;
		}
	}

	result = (result / float(blurSize * blurSize));

	ambient *= result;
	
	//addBlur
	float4 finalColor = float4(saturate(ambient + diffuseSpecular), 1);
	//finalColor = txSDO.Sample(samLinear, Input.Tex).a;
	return finalColor;
}