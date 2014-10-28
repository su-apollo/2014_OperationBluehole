

//--------------------------------------------------------------------------------------
// Globals
//--------------------------------------------------------------------------------------
cbuffer ConstantBuffer : register(b0)
{
	float4x4 mInverseProj;
	float4 vEye;
	float4 vNearFar;
	float4 vLightPos[2];
	float4 vLightColor[2];
	float4 vLightRange[2];
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
	float4 ScreenPos : TEXCOORD1;
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
	
	//for depth
	float n = vNearFar.x;
	float f = vNearFar.y;
	float z = (2.0 * n) / (f + n - depth.x * (f - n));
	
	// reconstruct pos
	float x = Input.ScreenPos.x;
	float y = Input.ScreenPos.y;
	float4 position = mul(float4(x, y, z, 1.0), mInverseProj);
	position.xyz /= position.www;
	// point
	position.w = 1.0f;

	float4 diffuseFactor = float4(0, 0, 0, 0);
	float4 specularFactor = float4(0, 0, 0, 0);;
	
	for ( int i = 0; i < 1 ; ++i )
	{
		// get lightDirection and distance
		float4 lightDir = vLightPos[i] - position;
		float distance = length(lightDir.xyz);
		lightDir /= distance;

		float3 attr = float3(0, 0, 1);
		float attrFactor = 1.0f / dot(attr, float3(1.0f, distance, distance*distance));

		//calculate diffuseFactor
		diffuseFactor += dot(lightDir, normal) * vLightColor[i] * attrFactor;

		//variables to calculate specular
		float4 reflection = normalize(reflect(lightDir, normal));
		float4 viewDir = normalize(position - vEye);

		//calculate specularFactor
		float specularResult = saturate(dot(viewDir, reflection));
		specularFactor += pow(specularResult, 1.0f) * vLightColor[i] * attrFactor;
		
	}

	specular *= specularFactor;
	diffuse *= diffuseFactor;

	float4 finalColor = 0;
	//finalColor = saturate(ambient + specular + diffuse);
	//finalColor = diffuse;
	finalColor = float4(z, z, z, 1);
	return finalColor;
}

