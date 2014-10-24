#pragma once
#include "Singleton.h"







class SamplerManager : public Singleton<SamplerManager>
{
public:
	SamplerManager();
	~SamplerManager();

	BOOL				Init();
	ID3D11SamplerState* GetLinearSampler() { return mSamplerLinear; }

private:

	BOOL				CreateLinearSampler();

	ID3D11SamplerState*		mSamplerLinear = NULL;

	// get from renderer
	ID3D11Device*           mD3DDevice = NULL;

	// get last error
	HRESULT hr = S_OK;
};

