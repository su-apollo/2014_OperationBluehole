
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
// SSAO
// find occlusion value from 8 sample kernels.
//--------------------------------------------------------------------------------------
float getOcclusion(float3x3 tbn, float4 position, float4 normal)
{
	float radius = vKernelVariables.x;

	float occlusion = 0.0f;
	for (int i = 0; i < 8; ++i)
	{
		//make sample kernels
		float3 sampleWorldPos = vSampleSphere[i].xyz;
		sampleWorldPos = mul(tbn, sampleWorldPos);

		float scale = float(i) / float(8);
		scale = lerp(0.1f, 1.0f, scale * scale);
		sampleWorldPos *= scale;
		
		sampleWorldPos = sampleWorldPos * radius + position.xyz;

		//project position
		float4 sampleProjected = mul(float4(sampleWorldPos, 1), mViewProj); // -1~1
		sampleProjected.xyz /= sampleProjected.w;
		float2 projCoord = float2(sampleProjected.x*0.5 + 0.5, 0.5 - sampleProjected.y*0.5); // 0~1

		//get original depth
		float	originalDepth = txDepth.Sample(samLinear, projCoord).x; // 0~1
		float4 originalWorldPos = mul(float4(sampleProjected.x, sampleProjected.y, originalDepth, 1), mInverseViewProj);
		originalWorldPos /= originalWorldPos.w;

		// 차폐에 기여하는가 검사. 같은 평면에 가까이 있는 점일 경우 차폐에 별로 기여하지 않는다고 봄.
		float rangeCheck = max(dot(normal.xyz, normalize(originalWorldPos.xyz - position.xyz)), 0);
		if (originalDepth == 1) rangeCheck = 0;

		// 차폐 여부 검사
		float dist = sampleProjected.z - originalDepth;
		if (dist < 0) rangeCheck = 0;

		// 거리에 따라 영향을 주도록.
		occlusion += saturate((radius*0.8 - dist) / radius) * rangeCheck;
		//occlusion += step(originalDepth, sampleProjected.z)* rangeCheck;
	}

	//more occluded means darker.
	occlusion = 1 - (occlusion / 8);

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

	float4 diffuseFactor = float4(0, 0, 0, 0);
	float4 specularFactor = float4(0, 0, 0, 0);

	for (int i = 0; i < 2; ++i)
	{
		// get lightDirection and distance
		float4 lightDir = vLightPos[i] - position;
		float distance = length(lightDir.xyz);
		lightDir /= distance;

		float3 attr = float3(0, 0, 1);
		float attrFactor = 1.0f - saturate((distance - vLightRange[i].y) / vLightRange[i].x);
		attrFactor = pow(attrFactor, 2);

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
	diffuse *= 0.8;

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

	float occlusion = getOcclusion(kernelTBN, position,normal);

	float4 finalColor = 0;
	finalColor = saturate(float4((diffuse + specular).xyz, occlusion));
	return finalColor;
}

	