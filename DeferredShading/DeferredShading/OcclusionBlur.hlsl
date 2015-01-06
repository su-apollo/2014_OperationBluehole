
//--------------------------------------------------------------------------------------
// Globals
//--------------------------------------------------------------------------------------

cbuffer ConstantBuffer : register(b0)
{
	matrix mInverseViewProj;
	float4 vEye;
	float4 vLightPos[50];
	float4 vLightColor[50];
	float4 vLightRange[50];
};


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
Texture2D txNormal : register(t2);
Texture2D txDiffuse : register(t3);
Texture2D txSpecular : register(t4);
Texture2D txDepth : register(t5);
SamplerState samLinear : register(s0);

//--------------------------------------------------------------------------------------
// Blur functions
// returns blurred bounced color and blurredOcclusion
//--------------------------------------------------------------------------------------
float4 SsdoBlur(float blurSize, float texelSize, PS_INPUT Input)
{
	float3 blurredBounce = float3(0, 0, 0);
		float  blurredOcclusion = 0.0f;

	float weight[25] = {
		0.031827f, 0.037541f, 0.039665f, 0.037541f, 0.031827f,
		0.037541f, 0.044281f, 0.046787f, 0.044281f, 0.037541f,
		0.039665f, 0.046787f, 0.049434f, 0.046787f, 0.039665f,
		0.037541f, 0.044281f, 0.046787f, 0.044281f, 0.037541f,
		0.031827f, 0.037541f, 0.039665f, 0.037541f, 0.031827f
	};
	float2 hlim = (float(-blurSize) * 0.5 + 0.5);

	[unroll]
	for (int i = 0; i < blurSize; ++i)
	{
		for (int j = 0; j < blurSize; ++j)
		{
			float2 offset = (hlim + float2(float(i), float(j))) * texelSize;
			
			blurredBounce += (txSSDO.Sample(samLinear, Input.Tex + offset * 2).xyz*weight[blurSize*i + j]);
			blurredOcclusion += (txSSDO.Sample(samLinear, Input.Tex + offset * 2).a*weight[blurSize*i + j]);
			
			//blurredBounce += txSSDO.Sample(samLinear, Input.Tex + offset).xyz;
			//blurredOcclusion += txSSDO.Sample(samLinear, Input.Tex + offset).a;
		}
	}
//	blurredBounce = (blurredBounce / float(blurSize * blurSize));
//	blurredOcclusion = (blurredOcclusion / float(blurSize * blurSize));
	return float4(blurredBounce, blurredOcclusion);
}

float4 main(PS_INPUT Input) : SV_TARGET
{
	float4 ssdo = txSSDO.Sample(samLinear, Input.Tex);
	float3 diffSpec = txDiffSpec.Sample(samLinear, Input.Tex).xyz;
	float3 ambient = float3(0.1f,0.1f, 0.1f);

	float2 texSize;
	txSSDO.GetDimensions(texSize.x, texSize.y);
	float2 texelSize = 1.0 / texSize;
	float blurSize = 5;

	float4 blurredSSDO = SsdoBlur(blurSize, texelSize, Input);

	// load G-buffer
	float4 normal = txNormal.Sample(samLinear, Input.Tex);
	float4 diffuse = txDiffuse.Sample(samLinear, Input.Tex);
	float4 specular = txSpecular.Sample(samLinear, Input.Tex);
	float4 depth = txDepth.Sample(samLinear, Input.Tex);

	normal = normal * 2 - 1;

	// reconstruct world pos
	float4 position;
	position.x = Input.Tex.x * 2 - 1;
	position.y = (1 - Input.Tex.y) * 2 - 1;
	position.z = depth.x;
	position.w = 1;
	position = mul(position, mInverseViewProj);
	position /= position.w;

	//light
	float4 diffuseFactor = float4(0, 0, 0, 0);
	float4 specularFactor = float4(0, 0, 0, 0);

	[unroll]
	for (int i = 0; i < 50; ++i)
	{
		// get lightDirection and distance
		float4 lightDir = vLightPos[i] - position;
		float distance = length(lightDir);
		lightDir /= distance;

		float3 attr = float3(0, 0, 1);
		float attrFactor = 1.0f - (max(distance - vLightRange[i].y,0) / vLightRange[i].x);

		//if (distance > vLightRange[i].x)
		//	attrFactor = 0.0f;

		float lDotN = dot(lightDir, normal);

		if (lDotN > 0.0f)
		{
			//calculate diffuseFactor
			diffuseFactor += lDotN * vLightColor[i] * attrFactor;

			//variables to calculate specular
			float4 reflection = reflect(-lightDir, normal);
			float4 viewDir = normalize(vEye-position);

			//calculate specularFactor
			float vDotN = saturate(dot(viewDir, reflection));
			specularFactor += pow(vDotN, 1.0f) * vLightColor[i] * attrFactor;
		}
		
	}

	// compute color
	diffuse += float4(blurredSSDO.xyz, 1);

	specular *= specularFactor;
	diffuse *= diffuseFactor;

	return (saturate(specular) + saturate(diffuse) + float4(ambient, 1)) * blurredSSDO.a;
}