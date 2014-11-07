
//--------------------------------------------------------------------------------------
// include / define
//--------------------------------------------------------------------------------------

#define FXAA_PC 1
#define FXAA_HLSL_5 1
#define FXAA_GREEN_AS_LUMA 1
#define FXAA_QUALITY__PRESET 12
#include "fxaa.h"

//--------------------------------------------------------------------------------------
// Textures and Samplers
//--------------------------------------------------------------------------------------

Texture2D txScene : register(t0);
SamplerState samLinear : register(s0);


float4 main() : SV_TARGET
{
	float2 fxaaFrame;
	txScene.GetDimensions(fxaaFrame.x, fxaaFrame.y);
	float fxaaSubpix = 0.75;
	float fxaaEdgeThreshold = 0.166;
	float fxaaEdgeThresholdMin = 0.0833;

	FxaaTex tex = { samLinear, txScene };
	
	//return FxaaPixelShader(Input.Tex, 0, tex, tex, tex, 1 / fxaaFrame, 0, 0, 0, fxaaSubpix, fxaaEdgeThreshold, fxaaEdgeThresholdMin, 0, 0, 0, 0);

	return float4(1.0f, 1.0f, 1.0f, 1.0f);
}