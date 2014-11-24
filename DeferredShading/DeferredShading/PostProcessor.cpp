#include "stdafx.h"
#include "PostProcessor.h"
#include "Renderer.h"
#include "App.h"
#include "RTManager.h"
#include "LightManager.h"
#include "Camera.h"
#include "SamplerManager.h"
#include "RSManager.h"

PostProcessor::PostProcessor()
{
}


PostProcessor::~PostProcessor()
{
	SafeRelease(mPixelShader);
	SafeRelease(mPSConstBuffer);
	SafeRelease(mVertexBuffer);
	SafeRelease(mIndexBuffer);
}

BOOL PostProcessor::Init()
{
	mD3DDevice = Renderer::GetInstance()->GetDevice();
	mD3DDeviceContext = Renderer::GetInstance()->GetDeviceContext();
	mBackBuffRTV = Renderer::GetInstance()->GetRenderTargetView();
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

	if (!CreateQuad())
	{
		MessageBox(hWnd, L"PostProcessor Quad Error!", L"Error!", MB_ICONINFORMATION | MB_OK);
		return FALSE;
	}
	if (!LoadNoiseTexture())
	{
		return FALSE;
	}

	return TRUE;
}


void PostProcessor::Render()
{
	RTManager::GetInstance()->SetRenderTargetToSDOBuff();
	LightPass();
	RTManager::GetInstance()->SetRenderTargetToSceneBuff();
	OccBlurPass();
	FXAAPass();
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

	//some variables
	mSampleSphere[0] = { 0.5381f, 0.4856f, 0.4319f, 1.0f };
	mSampleSphere[1] = { 0.1379f, -0.7486f, 0.6430f, 1.0f };
	mSampleSphere[2] = { -0.3371f, -0.5679f, 0.2057f, 1.0f };
	mSampleSphere[3] = { -0.6999f, 0.2451f, 0.9019f, 1.0f };
	mSampleSphere[4] = { 0.4689f, -0.1598f, 0.8547f, 1.0f };
	mSampleSphere[5] = { 0.2560f, 0.8069f, 0.0843f, 1.0f };
	mSampleSphere[6] = { -0.4146f, -0.1402f, 0.1762f, 1.0f };
	mSampleSphere[7] = { -0.7100f, 0.3924f, 0.7344f, 1.0f };
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

	hr = CompileShaderFromFile(mOccBlurShaderPath, mPixelShaderMain, mPixelShaderModel, &pPSBlob);
	if (FAILED(hr))
		return FALSE;

	hr = mD3DDevice->CreatePixelShader(pPSBlob->GetBufferPointer(),
		pPSBlob->GetBufferSize(), NULL, &mOccBlurShader);

	SafeRelease(pPSBlob);
	if (FAILED(hr))
		return FALSE;

	hr = CompileShaderFromFile(mFXAAShaderPath, mPixelShaderMain, mPixelShaderModel, &pPSBlob);
	if (FAILED(hr))
		return FALSE;

	hr = mD3DDevice->CreatePixelShader(pPSBlob->GetBufferPointer(),
		pPSBlob->GetBufferSize(), NULL, &mFXAAShader);

	SafeRelease(pPSBlob);
	if (FAILED(hr))
		return FALSE;

	return TRUE;
}

HRESULT PostProcessor::CompileShaderFromFile(WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut)
{
	DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_PACK_MATRIX_COLUMN_MAJOR;

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

void PostProcessor::RenderCleanUp()
{
	mD3DDeviceContext->VSSetShader(0, 0, 0);
	mD3DDeviceContext->PSSetShader(0, 0, 0);
	mD3DDeviceContext->OMSetRenderTargets(0, 0, 0);
	ID3D11ShaderResourceView* nullSRV[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
	mD3DDeviceContext->VSSetShaderResources(0, 8, nullSRV);
	mD3DDeviceContext->PSSetShaderResources(0, 8, nullSRV);
}

BOOL PostProcessor::LoadNoiseTexture()
{
	hr = D3DX11CreateShaderResourceViewFromFile(mD3DDevice, NOISE_TEXTURE, NULL, NULL, &mTextureNoise, NULL);
	if (FAILED(hr))
		return FALSE;
	return TRUE;
}

void PostProcessor::ChangeKernelRadius(float radius)
{
	mKernelRadius += radius;
}

void PostProcessor::LightPass()
{
	// set render target view
	//mD3DDeviceContext->OMSetRenderTargets(1, &mBackBuffRTV, NULL);

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
	ID3D11ShaderResourceView* normalTexRV = RTManager::GetInstance()->GetNormalTexRV();
	ID3D11ShaderResourceView* diffuseTexRV = RTManager::GetInstance()->GetDiffuseTexRV();
	ID3D11ShaderResourceView* specularTexRV = RTManager::GetInstance()->GetSpecularTexRV();
	ID3D11ShaderResourceView* depthTexRV = RTManager::GetInstance()->GetDepthTexRV();

	// set constbuff
	PostProcessorConstantBuffer pcb;
	//D3DXMATRIX matInverseProj = Camera::GetInstance()->GetMatInverseProj();
	D3DXMATRIX matInverseViewProj = Camera::GetInstance()->GetMatInverseViewProj();
	D3DXMatrixTranspose(&matInverseViewProj, &matInverseViewProj);

	D3DXMATRIX matInverseProj = Camera::GetInstance()->GetMatInverseProj();
	D3DXMatrixTranspose(&matInverseProj, &matInverseProj);
	D3DXMATRIX matView = Camera::GetInstance()->GetMatView();
	D3DXMATRIX matViewProj = Camera::GetInstance()->GetMatProj();
	matViewProj = matView * matViewProj;
	D3DXMatrixTranspose(&matView, &matView);
	D3DXMatrixTranspose(&matViewProj, &matViewProj);

	pcb.mInverseViewProj = matInverseViewProj;
	pcb.mViewProj = matViewProj;
	pcb.mInverseProj = matInverseProj;
	pcb.vEye = D3DXVECTOR4(Camera::GetInstance()->GetPosition(), 1);
	for (int i = 0; i < MAX_LIGHT; ++i)
	{
		PLightPointer light = LightManager::GetInstance()->mPLightList[i];
		pcb.vLightPos[i] = light->mPos;
		pcb.vLightColor[i] = light->mColor;
		pcb.vLightRange[i] = D3DXVECTOR4(light->mRange, light->mFullRange, light->mRange, 1);
	}
	pcb.vKernelVariables = D3DXVECTOR4(mKernelRadius, 0, 0, 0);
	for (int i = 0; i < KERNEL_NUM; ++i)
	{
		pcb.vSampleSphere[i] = mSampleSphere[i];
	}
	mD3DDeviceContext->UpdateSubresource(mPSConstBuffer, 0, NULL, &pcb, 0, 0);

	mD3DDeviceContext->PSSetShaderResources(0, 1, &normalTexRV);
	mD3DDeviceContext->PSSetShaderResources(1, 1, &diffuseTexRV);
	mD3DDeviceContext->PSSetShaderResources(2, 1, &specularTexRV);
	mD3DDeviceContext->PSSetShaderResources(3, 1, &depthTexRV);
	mD3DDeviceContext->PSSetShaderResources(4, 1, &mTextureNoise);

	ID3D11SamplerState* linearSampler = SamplerManager::GetInstance()->GetLinearSampler();
	mD3DDeviceContext->PSSetSamplers(0, 1, &linearSampler);

	// draw
	mD3DDeviceContext->DrawIndexed(6, 0, 0);
}

void PostProcessor::OccBlurPass()
{
	mD3DDeviceContext->PSSetShader(mOccBlurShader, NULL, 0);

	ID3D11ShaderResourceView* TexRV = RTManager::GetInstance()->GetSDOTexRV();
	ID3D11ShaderResourceView* TexRV0 = RTManager::GetInstance()->GetDiffSpecTexRV();

	mD3DDeviceContext->PSSetShaderResources(0, 1, &TexRV);
	mD3DDeviceContext->PSSetShaderResources(1, 1, &TexRV0);


	// draw
	mD3DDeviceContext->DrawIndexed(6, 0, 0);
}

void PostProcessor::FXAAPass()
{
	// last pass
	mD3DDeviceContext->OMSetRenderTargets(1, &mBackBuffRTV, NULL);

	// set pixel shader
	mD3DDeviceContext->PSSetShader(mFXAAShader, NULL, 0);

	ID3D11ShaderResourceView* TexRV = RTManager::GetInstance()->GetSceneTexRV();
	mD3DDeviceContext->PSSetShaderResources(0, 1, &TexRV);

	// draw
	mD3DDeviceContext->DrawIndexed(6, 0, 0);
}
