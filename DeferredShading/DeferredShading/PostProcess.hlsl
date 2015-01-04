
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

struct OUT
{
	float4 SSDO : SV_TARGET0;
	float4 DiffSpec : SV_TARGET1;
};

//--------------------------------------------------------------------------------------
// SSAO
// find occlusion value from 8 sample kernels.
//--------------------------------------------------------------------------------------
float getOcclusion(float3x3 tbn, float4 position, float4 normal)
{
	float radius = vKernelVariables.x;
	float occlusion = 0.0f;
	float bounceRange = 50.0f;

	[unroll]
	for (int i = 0; i < 8; ++i)
	{
		//make sample kernels
		float3 sampleWorldPos = vSampleSphere[i].xyz;
		sampleWorldPos = mul(tbn, sampleWorldPos);

		float scale = float(i) / float(8);
		scale = lerp(0.1f, 1.0f, scale * scale);
		sampleWorldPos *= scale;
		
		sampleWorldPos = - sampleWorldPos * radius + position.xyz;

		//project position
		float4 sampleProjected = mul(float4(sampleWorldPos, 1), mViewProj); // -1~1
		sampleProjected.xyz /= sampleProjected.w;
		float2 projCoord = float2(sampleProjected.x*0.5 + 0.5, 0.5 - sampleProjected.y*0.5); // 0~1

		// get original depth
		float	originalDepth = txDepth.Sample(samLinear, projCoord).x; // 0~1
		float4 originalWorldPos = mul(float4(sampleProjected.x, sampleProjected.y, originalDepth, 1), mInverseViewProj);
		originalWorldPos /= originalWorldPos.w;

		// the sample point too close to original point must be ignored.
		float rangeCheck = max(dot(normal.xyz, normalize(originalWorldPos.xyz - position.xyz)), 0);
		if (originalDepth == 1) rangeCheck = 0;

		// check if it's an occluder
		float dist = sampleProjected.z - originalDepth;
		//get a vector from original pos to position pos
		float4 originalToPos = position - originalWorldPos;
		//get a distance
		float distance = length(originalToPos);
		if (dist < 0) rangeCheck = 0;

		if (sampleProjected.x<-1)rangeCheck = 0;
		if (sampleProjected.y<-1)rangeCheck = 0;
		if (sampleProjected.x>1)rangeCheck = 0;
		if (sampleProjected.y>1)rangeCheck = 0;

		if (distance > bounceRange) rangeCheck = 0;

		// occlusion value decreases with distance.
		occlusion += saturate((radius*0.8 - distance) / radius) * rangeCheck;
	}

	//more occluded means darker.
	occlusion = 1 - (occlusion / 8);

	return occlusion;
}

float3 ComputeSSDO(float3x3 tbn, float4 position, float4 normal)
{
	float3 bouncedColor = float3(0, 0, 0);
	float radius = 15.0f;
	float bounceRange = 10.0f;

	[unroll]
	for (int i = 0; i < 8; ++i)
	{
		//make sample kernels
		float3 sampleWorldPos = vSampleSphere[i].xyz;
		sampleWorldPos = mul(tbn, sampleWorldPos);

		float scale = float(i) / float(8);
		scale = lerp(0.3f, 1.0f, scale * scale);
		sampleWorldPos *= scale;

		sampleWorldPos = -sampleWorldPos * radius + position.xyz;

		//project position
		float4 sampleProjected = mul(float4(sampleWorldPos, 1), mViewProj); // -1~1
		sampleProjected.xyz /= sampleProjected.w;
		float2 projCoord = float2(sampleProjected.x*0.5 + 0.5, 0.5 - sampleProjected.y*0.5); // 0~1

		//get original depth
		float	originalDepth = txDepth.Sample(samLinear, projCoord).x; // 0~1
		float4 originalWorldPos = mul(float4(sampleProjected.x, sampleProjected.y, originalDepth, 1), mInverseViewProj);
		originalWorldPos /= originalWorldPos.w;

		//get original normal
		float4 originalWorldNormal = txNormal.Sample(samLinear, projCoord);

		//get original diffuse color
		float4 originalColor = txDiffuse.Sample(samLinear, projCoord);
		//get a vector from original pos to position pos
		float4 originalToPos = position - originalWorldPos;
		//get a distance
		float distance = length(originalToPos);
		//normalize vector
		originalToPos = normalize(originalToPos);

		// the sample point too close to original point must be ignored.
		// scale해주는게 나을듯.
		float rangeCheck = max(dot(normal.xyz, normalize(originalWorldPos.xyz - position.xyz)), 0);
		if (originalDepth == 1) rangeCheck = 0;

		// check if it's an occluder
		float dist = sampleProjected.z - originalDepth;
		if (dist < 0) rangeCheck = 0;

		if (sampleProjected.x<-1)rangeCheck = 0;
		if (sampleProjected.y<-1)rangeCheck = 0;
		if (sampleProjected.x>1)rangeCheck = 0;
		if (sampleProjected.y>1)rangeCheck = 0;

		if (distance > bounceRange) rangeCheck = 0;

		bouncedColor += originalColor.xyz*saturate((radius - distance) / radius) *rangeCheck;
	}
	return saturate(bouncedColor);
}

/*
//--------------------------------------------------------------------------------------
// SSDO : occlusion + one indirect bounce
// return float4(indirect color.xyz,occlusion)
//--------------------------------------------------------------------------------------
float4 ComputeSSDO(float3x3 tbn, float4 position, float4 normal)
{
	float3 bouncedColor = float3(0,0,0);
	float occlusion = 0.0f;
	float radius = vKernelVariables.x;
	float bounceRange = 50.0f;

	[unroll]
	for (int i = 0; i < 8; ++i)
	{
		//make sample kernels
		float3 sampleWorldPos = vSampleSphere[i].xyz;
		sampleWorldPos = mul(tbn, sampleWorldPos);

		float scale = float(i) / float(8);
		scale = lerp(0.3f,1.0f, scale * scale);
		sampleWorldPos *= scale;
		
		sampleWorldPos = - sampleWorldPos * radius + position.xyz;

		//project position
		float4 sampleProjected = mul(float4(sampleWorldPos, 1), mViewProj); // -1~1
		sampleProjected.xyz /= sampleProjected.w;
		float2 projCoord = float2(sampleProjected.x*0.5 + 0.5, 0.5 - sampleProjected.y*0.5); // 0~1
			
		//get original depth
		float	originalDepth = txDepth.Sample(samLinear, projCoord).x; // 0~1
		float4 originalWorldPos = mul(float4(sampleProjected.x, sampleProjected.y, originalDepth, 1), mInverseViewProj);
		originalWorldPos /= originalWorldPos.w;

		//get original normal
		float4 originalWorldNormal = txNormal.Sample(samLinear, projCoord);

		//get original diffuse color
		float4 originalColor = txDiffuse.Sample(samLinear, projCoord);

		//get a vector from original pos to position pos
		float4 originalToPos = position - originalWorldPos;
		//get a distance
		float distance = length(originalToPos);
		//normalize vector
		originalToPos = normalize(originalToPos);

		// the sample point too close to original point must be ignored.
		// scale해주는게 나을듯.
		float rangeCheck = max(dot(normal.xyz, normalize(originalWorldPos.xyz - position.xyz)), 0);
		if (originalDepth == 1) rangeCheck = 0;

		// check if it's an occluder
		float dist = sampleProjected.z - originalDepth;
		if (dist < 0) rangeCheck = 0;
		
		if (sampleProjected.x<-1)rangeCheck = 0;
		if (sampleProjected.y<-1)rangeCheck = 0;
		if (sampleProjected.x>1)rangeCheck = 0;
		if (sampleProjected.y>1)rangeCheck = 0;

		if (distance > bounceRange) rangeCheck = 0;
		

		bouncedColor += originalColor.xyz*max(dot(originalWorldNormal, originalToPos), 0)*max(((radius - distance) / radius),0) *rangeCheck;
		occlusion += saturate((radius*0.8 - dist) / radius) * rangeCheck;

	}

	//more occluded means darker.
	occlusion = 1 - (occlusion / 8);

	return saturate(float4(bouncedColor,occlusion));
}

*/
//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------

OUT main(PS_INPUT Input) : SV_TARGET
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

	[unroll]
	for (int i = 0; i < 2; ++i)
	{
		// get lightDirection and distance
		float4 lightDir = vLightPos[i] - position;
		float distance = length(lightDir);
		lightDir /= distance;

		float3 attr = float3(0, 0, 1);
		float attrFactor = 1.0f - (max(distance - vLightRange[i].y,0) / vLightRange[i].x);
		attrFactor = pow(attrFactor, 2);

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
			float vDotN = dot(viewDir, reflection);
			specularFactor += pow(vDotN, 1.0f) * vLightColor[i] * attrFactor;
		}
		
	}

	specular *= specularFactor;
	diffuse *= diffuseFactor;

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

	// only SSAO
	float occlusion = getOcclusion(kernelTBN, position,normal);

	float3 ssdo = ComputeSSDO(kernelTBN, position, normal);

	OUT output;
	output.SSDO = float4(ssdo, occlusion);
	output.DiffSpec = float4(saturate((0.8*diffuse+0.5*specular).xyz), 1);

	return output;
}

	