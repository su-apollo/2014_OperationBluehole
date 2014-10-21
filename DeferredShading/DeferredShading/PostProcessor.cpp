#include "stdafx.h"
#include "PostProcessor.h"
#include "Renderer.h"
#include "App.h"


PostProcessor::PostProcessor()
{
}


PostProcessor::~PostProcessor()
{
}

BOOL PostProcessor::Init()
{
	mD3DDevice = Renderer::GetInstance()->GetDevice();
	mD3DDeviceContext = Renderer::GetInstance()->GetDeviceContext();
	mRenderTargetView = Renderer::GetInstance()->GetRenderTargetView();
	HWND hWnd = App::GetInstance()->GetHandleMainWindow();

	if (!CompilePixelShader())
	{
		MessageBox(hWnd, L"PostProcessor CompileShader Error!", L"Error!", MB_ICONINFORMATION | MB_OK);
		return FALSE;
	}

	if (!CreateConstBuffer())
	{
		MessageBox(hWnd, L"PostProcessor CreateConstBuffer Error!", L"Error!", MB_ICONINFORMATION | MB_OK);
		return FALSE;
	}

	if (!CreateSamplerLinear())
	{
		MessageBox(hWnd, L"PostProcessor CreateSampler Error!", L"Error!", MB_ICONINFORMATION | MB_OK);
		return FALSE;
	}

	return TRUE;
}


void PostProcessor::Render()
{
	mD3DDeviceContext->OMSetRenderTargets(1, &mRenderTargetView, NULL);





}

HRESULT PostProcessor::CompileShaderFromFile(WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut)
{
	DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;

#if defined( DEBUG ) || defined( _DEBUG )
	dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif

	ID3DBlob* pErrorBlob;
	hr = D3DX11CompileFromFile(szFileName, NULL, NULL, szEntryPoint, szShaderModel,
		dwShaderFlags, 0, NULL, ppBlobOut, &pErrorBlob, NULL);
	if (FAILED(hr))
	{
		if (pErrorBlob != NULL)
			OutputDebugStringA((char*)pErrorBlob->GetBufferPointer());
		if (pErrorBlob) pErrorBlob->Release();
		return hr;
	}
	if (pErrorBlob) pErrorBlob->Release();

	return S_OK;
}

BOOL PostProcessor::CreateConstBuffer()
{
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	// Create the constant buffer
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.ByteWidth = sizeof(PostProcessorConstantBuffer);
	hr = mD3DDevice->CreateBuffer(&bd, NULL, &mPSConstBuffer);
	if (FAILED(hr))
		return FALSE;

	return TRUE;
}

BOOL PostProcessor::CompilePixelShader()
{
	ID3DBlob* pPSBlob = NULL;
	hr = CompileShaderFromFile(mPixelShaderPath, mPixelShaderMain, mPixelShaderModel, &pPSBlob);
	if (FAILED(hr))
		return FALSE;

	hr = mD3DDevice->CreatePixelShader(pPSBlob->GetBufferPointer(),
		pPSBlob->GetBufferSize(), NULL, &mPixelShader);

	SafeRelease(pPSBlob);
	if (FAILED(hr))
		return FALSE;

	return TRUE;
}

BOOL PostProcessor::CreateSamplerLinear()
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

BOOL PostProcessor::CreateQuad()
{
	QuadVertex verts[4] =
	{
		{ D3DXVECTOR4(1, 1, 1, 1), D3DXVECTOR2(1, 0) },
		{ D3DXVECTOR4(1, -1, 1, 1), D3DXVECTOR2(1, 1) },
		{ D3DXVECTOR4(-1, -1, 1, 1), D3DXVECTOR2(0, 1) },
		{ D3DXVECTOR4(-1, 1, 1, 1), D3DXVECTOR2(0, 0) }
	};

	D3D11_BUFFER_DESC desc;
	// immutable
	desc.Usage = D3D11_USAGE_IMMUTABLE;
	desc.ByteWidth = sizeof(verts);
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA initData;
	initData.pSysMem = verts;
	initData.SysMemPitch = 0;
	initData.SysMemSlicePitch = 0;
	mD3DDevice->CreateBuffer(&desc, &initData, &mVertexBuffer);
	if (FAILED(hr))
		return FALSE;
	
	unsigned short indices[6] = { 0, 1, 2, 2, 3, 0 };

	desc.Usage = D3D11_USAGE_IMMUTABLE;
	desc.ByteWidth = sizeof(indices);
	desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	desc.CPUAccessFlags = 0;
	initData.pSysMem = indices;
	mD3DDevice->CreateBuffer(&desc, &initData, &mIndexBuffer);
	if (FAILED(hr))
		return FALSE;

	return TRUE;
}

