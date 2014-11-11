
//--------------------------------------------------------------------------------------
// Globals
//--------------------------------------------------------------------------------------
// vKernelVaraibles : x) kernelRadius
// vLightRange : x) lightRange y)full powere range

cbuffer ConstantBuffer : register(b0)
{
	matrix mInverseViewProj;
	matrix mViewProj;
	matrix mInverseProj;
	float4 vEye;
	float4 vLightPos[2];
	float4 vLightColor[2];
	float4 vLightRange[2];
	float4 vKernelVariables;
	float3 vSampleSphere[8];
};

//--------------------------------------------------------------------------------------
// Textures and Samplers
//--------------------------------------------------------------------------------------

//배열로 관리할 수 있을 것 같은데
Texture2D txNormal : register(t0);
Texture2D txDiffuse : register(t1);
Texture2D txSpecular : register(t2);
Texture2D txDepth : register(t3);
Texture2D txNoise : register(t4);
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
// SSDO
// compute the product of incoming radiance, visibility and the diffuse BRDF.
// make kernel that covers uniform range. ( 360/kernelNum ) but why uniform? to use BRDF?

//--------------------------------------------------------------------------------------
float getOcclusion(float3x3 tbn, float4 position, float4 normal)
{
	float radius = vKernelVariables.x;
	float occlusion = 0.0f;

	//make kernels here!
	for (int i = 0; i < 8; ++i)
	{
		//make sample kernels
		float3 sampleWorldPos = vSampleSphere[i].xyz;
		sampleWorldPos = mul(tbn, sampleWorldPos);
	}

	return occlusion;
}

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------

float4 main(PS_INPUT Input) : SV_TARGET
{
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


	float2 texSize;
	float2 noiseTexSize;
	txDiffuse.GetDimensions(texSize.x, texSize.y);
	txNoise.GetDimensions(noiseTexSize.x, noiseTexSize.y);

	float2 noiseTexCoords = texSize / noiseTexSize;
	noiseTexCoords *= Input.Tex;
	float4 noise = txNoise.Sample(samLinear, noiseTexCoords);

	float3 randomVector = noise.xyz *  2.0 - 1.0; //-1~1
	randomVector.z = 0;

	float3 tangent = normalize(randomVector - normal.xyz*dot(randomVector, normal.xyz));
	float3 bitangent = cross(tangent, normal.xyz);
	float3x3 kernelTBN = float3x3(tangent, bitangent, normal.xyz);

	float4 finalColor = 0;
	return finalColor;
}