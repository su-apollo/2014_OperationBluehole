

//--------------------------------------------------------------------------------------
// Globals
//--------------------------------------------------------------------------------------
cbuffer ConstantBuffer : register(b0)
{
	matrix mInverseViewProj;
	matrix mProj;
	matrix mInverseProj;
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
//--------------------------------------------------------------------------------------
/*
float getOcclusion(float3x3 tbn, float4 position)
{
	float3 sample_sphere[8] = {
		float3(0.5381, -0.4856, 0.4319), float3(0.1379, 0.2486, 0.4430),
		float3(0.3371, 0.5679, 0.0057), float3(-0.6999, -0.2451, -0.0019),
		float3(0.4689, -0.1598, 0.8547), float3(0.2560, 0.8069, 0.1843),
		float3(-0.4146, 0.1402, 0.0762), float3(-0.7100, -0.1924, 0.7344)
	};

	float radius = 0.5f;

	float occlusion = 0.0f;

	for (int i = 0; i < 8; ++i)
	{
		//make sample kernels
		float3 sampleViewPosition = sample_sphere[i]; 
		sampleViewPosition = mul(tbn, sampleViewPosition);
		sampleViewPosition = sampleViewPosition * radius + position.xyz;

		//project position
		float4 sampleProjected = mul(mProj, float4(sampleViewPosition, 1)); // -1~1
		sampleProjected.xy /= sampleProjected.w;
		float2 projected = float2(sampleProjected.x, sampleProjected.y);
		sampleProjected.xy = sampleProjected.xy*0.5 + 0.5; 

		//get original depth
		float sampleOriginalDepth = txDepth.Sample(samLinear, sampleProjected.xy).x;
		float4 originalViewPos = mul(float4(projected.x, projected.y, sampleOriginalDepth, 1), mInverseProj);
		originalViewPos /= originalViewPos.w;

		//optional calculation? sampleViewPosition.z > originalPos.z면 차폐된것이다.
		occlusion += step(originalViewPos.z, sampleViewPosition.z);
	}

	//more occluded means darker.
	occlusion = 1 - (occlusion / 8);
	return occlusion;
}
*/
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

	normal = normal * 2 - 1;
	
	// reconstruct pos
	float4 position;
	position.x = Input.Tex.x * 2 - 1;
	position.y = (1 - Input.Tex.y) * 2 - 1;
	position.z = depth.x;
	position.w = 1;
	position = mul(position, mInverseViewProj);
	position /= position.w;
	
	float4 diffuseFactor = float4(0, 0, 0, 0);
	float4 specularFactor = float4(0, 0, 0, 0);;
	
	for ( int i = 0; i < 1 ; ++i )
	{
		// get lightDirection and distance
		float4 lightDir = vLightPos[i] - position;
		float distance = length(lightDir.xyz);
		lightDir /= distance;

		float3 attr = float3(0, 0, 1);
		float attrFactor = 1.0f - saturate((distance-0.1f) / vLightRange[i].x);
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


	//SSAO
	//texture size 어떻게 얻지? 일단 숫자로 넣어놨지만 찾아서 바꿀 것.
	float2 noiseTexCoords = float2(1024.0f, 768.0f) / float2(128.0f, 128);
	noiseTexCoords *= Input.Tex;
	float4 noise = txNoise.Sample(samLinear, noiseTexCoords);
	
	//get view position
	float4 viewPosition;
	viewPosition.x = Input.Tex.x * 2 - 1;
	viewPosition.y = (1 - Input.Tex.y) * 2 - 1;
	viewPosition.z = depth.x;
	viewPosition.w = 1;
	viewPosition = mul(viewPosition, mInverseProj);
	viewPosition /= viewPosition.w;


	/*
	//make randomVector -1 ~ 1
	float3 randomVector = noise.xyz *  2.0 - 1.0;
	float3 tangent = normalize(randomVector - normal.xyz*dot(randomVector, normal.xyz));
	float3 bitangent = cross(tangent, normal.xyz);
	float3x3 kernelTBN = float3x3(tangent, bitangent, normal.xyz);


	float occlusionFactor = getOcclusion(kernelTBN, viewPosition);
	*/

	float4 finalColor = 0;
	finalColor = saturate(ambient + specular*4 + diffuse);
	//finalColor = specular * 4;
	//finalColor = float4(viewPosition.xxx, 1);
	//finalColor = float4(depth.xxx,1);
	//finalColor = float4(z, z, z, 1);
	return finalColor;
}

