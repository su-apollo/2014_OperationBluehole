

//--------------------------------------------------------------------------------------
// Textures and Samplers
//--------------------------------------------------------------------------------------
Texture2D txDiffuse : register(t0);
Texture2D txNormal : register(t1);
Texture2D txSpecular : register(t2);

SamplerState samLinear : register(s0);


//--------------------------------------------------------------------------------------
// Input / Output structures
//--------------------------------------------------------------------------------------
struct PS_INPUT
{
	float4 Pos : SV_POSITION;
	float2 Tex : TEXCOORD0;
	float3 Tang : TEXCOORD1;
	float3 Norm : TEXCOORD2;
	float4 WorldPos : TEXCOORD3;
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
	float3 tangentNormal = txNormal.Sample(samLinear, Input.Tex).xyz;
	//convert 0 ~ 1 to -1 ~ +1
	tangentNormal = normalize(tangentNormal * 2 - 1);

	float3 bitangent = cross(Input.Norm, Input.Tang);
	float3x3 TBN = float3x3(normalize(Input.Tang), normalize(bitangent), normalize(Input.Norm));
	float4 normal = float4(mul(tangentNormal, TBN), 1);
	normal = normal * 0.5 + 0.5;

	GBuffer output;
	output.normal = normal;
	output.diffuse = txDiffuse.Sample(samLinear, Input.Tex);
	output.specular = txSpecular.Sample(samLinear, Input.Tex);

	//pointLihgt추가 : 정확히는 vLightPos여야.
	//WorldPos가 있어야 하기 때문에. diffuse도 여기서 계산해야 할 것 같다.??
	float4 lightDir = Input.WorldPos - vLightPos[0];
	float distance = length(lightDir);
	lightDir /= distance;
	output.diffuse *= saturate(dot(-lightDir, normal))* 0.5 * vLightColor[0];


	//variables to calculate specular
	float4 reflection = normalize(reflect(-lightDir, normal));
	float4 viewDir = normalize(Input.WorldPos - vEye);

	float4 specularResult = 0;

	specularResult = saturate(dot(viewDir, reflection));
	specularResult = pow(specularResult, 1.0f);
	specularResult *= output.specular*vLightColor[0];

	output.specular = specularResult;

	//output.normal = float4(Input.Norm, 1);
	//output.normal = float4(Input.Tang, 1);

	return output;
}

