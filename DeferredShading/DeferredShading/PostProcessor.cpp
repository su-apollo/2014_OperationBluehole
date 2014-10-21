#include "stdafx.h"
#include "PostProcessor.h"
#include "Renderer.h"
#include "App.h"
#include "GBuffManager.h"
#include "LightManager.h"
#include "Camera.h"


PostProcessor::PostProcessor()
{
}


PostProcessor::~PostProcessor()
{
	SafeRelease(mPixelShader);
	SafeRelease(mPSConstBuffer);
	SafeRelease(mVertexBuffer);
	SafeRelease(mIndexBuffer);
	SafeRelease(mSamplerLinear);
}

BOOL PostProcessor::Init()
{
	mD3DDevice = Renderer::GetInstance()->GetDevice();
	mD3DDeviceContext = Renderer::GetInstance()->GetDeviceContext();
	mRenderTargetView = Renderer::GetInstance()->GetRenderTargetView();
	mDepthStencilView = Renderer::GetInstance()->GetDepthStencilView();
	HWND hWnd = App::GetInstance()->GetHandleMainWindow();

	if (!CompileShader())
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

	if (!CreateQuad())
	{
		MessageBox(hWnd, L"PostProcessor Quad Error!", L"Error!", MB_ICONINFORMATION | MB_OK);
		return FALSE;
	}

	return TRUE;
}


void PostProcessor::Render()
{
	// set render target view
	mD3DDeviceContext->OMSetRenderTargets(1, &mRenderTargetView, NULL);

	// clear
	float ClearColor[4] = { 0.0f, 0.125f, 0.3f, 1.0f };
	mD3DDeviceContext->ClearRenderTargetView(mRenderTargetView, ClearColor);

	// set lay out
	mD3DDeviceContext->IASetInputLayout(mVertexLayout11);

	// set vertex
	UINT stride = sizeof(QuadVertex);
	UINT offset = 0;
	mD3DDeviceContext->IASetVertexBuffers(0, 1, &mVertexBuffer, &stride, &offset);

	// set index
	mD3DDeviceContext->IASetIndexBuffer(mIndexBuffer, DXGI_FORMAT_R16_UINT, 0);

	// set primitive
	mD3DDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// set shader
	mD3DDeviceContext->VSSetShader(mVertexShader, NULL, 0);
	mD3DDeviceContext->PSSetShader(mPixelShader, NULL, 0);
	mD3DDeviceContext->PSSetConstantBuffers(0, 1, &mPSConstBuffer);

	// set Gbuff
	ID3D11ShaderResourceView* normalTexRV = GBuffManager::GetInstance()->GetNormalTexRV();
	ID3D11ShaderResourceView* albedoTexRV = GBuffManager::GetInstance()->GetAlbedoTexRV();

	// set constbuff
	PostProcessorConstantBuffer pcb;
	DLightPointer light1 = LightManager::GetInstance()->mDLightList[0];
	DLightPointer light2 = LightManager::GetInstance()->mDLightList[1];
	pcb.vEye = D3DXVECTOR4(0, 0, 0, 1);
	pcb.vLightDir[0] = light1->GetDirection();
	pcb.vLightDir[1] = light2->GetDirection();
	pcb.vLightColor[0] = light1->GetColor();
	pcb.vLightColor[1] = light2->GetColor();
	mD3DDeviceContext->UpdateSubresource(mPSConstBuffer, 0, NULL, &pcb, 0, 0);

	mD3DDeviceContext->PSSetShaderResources(0, 1, &normalTexRV);
	mD3DDeviceContext->PSSetShaderResources(1, 1, &albedoTexRV);
	mD3DDeviceContext->PSSetSamplers(0, 1, &mSamplerLinear);

	// draw
	mD3DDeviceContext->DrawIndexed(6, 0, 0);
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

BOOL PostProcessor::CompileShader()
{
	// vertex shader
	ID3DBlob* pVSBlob = NULL;
	hr = CompileShaderFromFile(mVertexShaderPath, mVertexShaderMain, mVertexShaderModel, &pVSBlob);
	if (FAILED(hr))
		return FALSE;

	hr = mD3DDevice->CreateVertexShader(pVSBlob->GetBufferPointer(),
		pVSBlob->GetBufferSize(), NULL, &mVertexShader);

	if (FAILED(hr))
	{
		SafeRelease(pVSBlob);
		return FALSE;
	}

	// input layout
	const D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	UINT numElements = ARRAYSIZE(layout);

	hr = mD3DDevice->CreateInputLayout(layout, numElements, pVSBlob->GetBufferPointer(),
		pVSBlob->GetBufferSize(), &mVertexLayout11);

	SafeRelease(pVSBlob);
	if (FAILED(hr))
		return FALSE;

	// pixel shader
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
		{ D3DXVECTOR3(1, 1, 1), D3DXVECTOR2(1, 0) },
		{ D3DXVECTOR3(1, -1, 1), D3DXVECTOR2(1, 1) },
		{ D3DXVECTOR3(-1, -1, 1), D3DXVECTOR2(0, 1) },
		{ D3DXVECTOR3(-1, 1, 1), D3DXVECTOR2(0, 0) }
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

