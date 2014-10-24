#include "stdafx.h"
#include "SamplerManager.h"
#include "Renderer.h"
#include "App.h"

SamplerManager::SamplerManager()
{
}


SamplerManager::~SamplerManager()
{
	SafeRelease(mSamplerLinear);
}

BOOL SamplerManager::Init()
{
	HWND hWnd = App::GetInstance()->GetHandleMainWindow();
	mD3DDevice = Renderer::GetInstance()->GetDevice();

	if (!CreateLinearSampler())
	{
		MessageBox(hWnd, L"CreateLinearSampler Error!", L"Error!", MB_ICONINFORMATION | MB_OK);
		return FALSE;
	}

	return TRUE;
}

BOOL SamplerManager::CreateLinearSampler()
{
	// Create the sample state
	D3D11_SAMPLER_DESC sampDesc;
	ZeroMemory(&sampDesc, sizeof(sampDesc));
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
	hr = mD3DDevice->CreateSamplerState(&sampDesc, &mSamplerLinear);
	if (FAILED(hr))
		return FALSE;

	return TRUE;
}
