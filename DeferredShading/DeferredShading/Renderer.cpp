#include "stdafx.h"
#include "Renderer.h"
#include "App.h"
#include "Logger.h"



Renderer::Renderer()
{
}


Renderer::~Renderer()
{
	DestroyDevice();
}

BOOL Renderer::Init()
{
	if (!CreateDevice(App::GetInstance()->GetHandleMainWindow()))
	{
		MessageBox(App::GetInstance()->GetHandleMainWindow(), L"CreateDevice Error!", L"Error!", MB_ICONINFORMATION | MB_OK);
		DestroyDevice();
		return FALSE;
	}

	if (!CompileShader())
	{
		MessageBox(App::GetInstance()->GetHandleMainWindow(), L"CompileShader Error!", L"Error!", MB_ICONINFORMATION | MB_OK);
		DestroyDevice();
		return FALSE;
	}
	
	mCube.FillBuffer();

	return TRUE;
}

BOOL Renderer::CreateDevice(HWND hWnd)
{
	HRESULT hr = S_OK;

	RECT rc;
	GetClientRect(hWnd, &rc);
	UINT width = rc.right - rc.left;
	UINT height = rc.bottom - rc.top;

	UINT createDeviceFlags = 0;
#ifdef _DEBUG
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	D3D_DRIVER_TYPE driverTypes[] =
	{
		D3D_DRIVER_TYPE_HARDWARE,
		D3D_DRIVER_TYPE_WARP,
		D3D_DRIVER_TYPE_REFERENCE,
	};
	UINT numDriverTypes = ARRAYSIZE(driverTypes);

	D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
	};
	UINT numFeatureLevels = ARRAYSIZE(featureLevels);

	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.BufferCount = 1;
	sd.BufferDesc.Width = width;
	sd.BufferDesc.Height = height;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = hWnd;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = TRUE;

	for (UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++)
	{
		mDriverType = driverTypes[driverTypeIndex];
		hr = D3D11CreateDeviceAndSwapChain(NULL, mDriverType, NULL, createDeviceFlags, featureLevels, numFeatureLevels,
			D3D11_SDK_VERSION, &sd, &mSwapChain, &mD3DDevice, &mFeatureLevel, &mD3DDeviceContext);
		if (SUCCEEDED(hr))
			break;
	}

	if (FAILED(hr))
		return FALSE;

	// Create a render target view
	ID3D11Texture2D* pBackBuffer = NULL;
	hr = mSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
	if (FAILED(hr))
		return FALSE;

	hr = mD3DDevice->CreateRenderTargetView(pBackBuffer, NULL, &mRenderTargetView);
	pBackBuffer->Release();
	if (FAILED(hr))
		return FALSE;

	// set rendertarget
	mD3DDeviceContext->OMSetRenderTargets(1, &mRenderTargetView, NULL);

	// Setup the viewport
	D3D11_VIEWPORT vp;
	vp.Width = (FLOAT)width;
	vp.Height = (FLOAT)height;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	mD3DDeviceContext->RSSetViewports(1, &vp);

	return TRUE;
}

void Renderer::DestroyDevice()
{
	SafeRelease(mVertexLayout11);
	SafeRelease(mVertexShader);
	SafeRelease(mPixelShader);

	if (mD3DDeviceContext)
		mD3DDeviceContext->ClearState();

	SafeRelease(mRenderTargetView);
	SafeRelease(mSwapChain);
	SafeRelease(mD3DDeviceContext);
	SafeRelease(mD3DDevice);
}

void Renderer::Render()
{
	//clear
	float ClearColor[4] = { 0.0f, 0.125f, 0.3f, 1.0f }; //red,green,blue,alpha
	mD3DDeviceContext->ClearRenderTargetView(mRenderTargetView, ClearColor);

	D3DXMATRIX matWorldViewProjection;
	D3DXVECTOR3 vLightDir;
	D3DXMATRIX matWorld;
	D3DXMATRIX matView;
	D3DXMATRIX matProj;

	//matProj = mCamera.GetMatProj();
	//matView = mCamera.GetMatView();

	// per frame cb update
	D3D11_MAPPED_SUBRESOURCE MappedResource;
	mD3DDeviceContext->Map(mPSPerFrame, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource);
	CB_PS_PER_FRAME* pPerFrame = (CB_PS_PER_FRAME*)MappedResource.pData;
	float fAmbient = 0.1f;
	pPerFrame->mLightDirAmbient = D3DXVECTOR4(vLightDir.x, vLightDir.y, vLightDir.z, fAmbient);
	mD3DDeviceContext->Unmap(mPSPerFrame, 0);

	mD3DDeviceContext->PSSetConstantBuffers(gCBPSPerFrameBind, 1, &mPSPerFrame);
	
	//IA setup
	mD3DDeviceContext->IASetInputLayout(mVertexLayout11);

	// Set the shaders
	mD3DDeviceContext->VSSetShader(mVertexShader, NULL, 0);
	mD3DDeviceContext->PSSetShader(mPixelShader, NULL, 0);

	matWorld = mCube.GetMatWorld();
	matProj = mCamera.GetMatProj();
	matView = mCamera.GetMatView();

	matWorldViewProjection = matWorld * matView * matProj;

	// VS Per object
	mD3DDeviceContext->Map(mVSPerObject, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource);
	CB_VS_PER_OBJECT* pVSPerObject = (CB_VS_PER_OBJECT*)MappedResource.pData;
	D3DXMatrixTranspose(&pVSPerObject->mWorldViewProj, &matWorldViewProjection);
	D3DXMatrixTranspose(&pVSPerObject->mWorld, &matWorld);
	mD3DDeviceContext->Unmap(mVSPerObject, 0);

	mD3DDeviceContext->VSSetConstantBuffers(gCBVSPerObjectBind, 1, &mVSPerObject);

	// PS Per object
	mD3DDeviceContext->Map(mPSPerObject, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource);
	CB_PS_PER_OBJECT* pPSPerObject = (CB_PS_PER_OBJECT*)MappedResource.pData;
	pPSPerObject->mObjectColor = D3DXVECTOR4(1, 1, 1, 1);
	mD3DDeviceContext->Unmap(mPSPerObject, 0);

	mD3DDeviceContext->PSSetConstantBuffers(gCBPSPerObjectBind, 1, &mPSPerObject);

	//dp call
	mD3DDeviceContext->DrawIndexed(36, 0, 0);

	// back to front
	mSwapChain->Present(0, 0);
}

HRESULT Renderer::CompileShaderFromFile(WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut)
{
	HRESULT hr = S_OK;

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

BOOL Renderer::CompileShader()
{
	if (!mD3DDevice)
		return FALSE;

	HRESULT hr = S_OK;

	ID3DBlob* pVSBlob = NULL;
	hr = CompileShaderFromFile(const_cast<WCHAR*>(VS_PATH), VS_MAIN, "vs_4_0_level_9_1", &pVSBlob);
	if (FAILED(hr))
		return FALSE;

	ID3DBlob* pPSBlob = NULL;
	hr = CompileShaderFromFile(const_cast<WCHAR*>(PS_PATH), PS_MAIN, "ps_4_0_level_9_1", &pPSBlob);
	if (FAILED(hr))
		return FALSE;

	hr = mD3DDevice->CreateVertexShader(pVSBlob->GetBufferPointer(),
		pVSBlob->GetBufferSize(), NULL, &mVertexShader);

	if (FAILED(hr))
		return FALSE;

	hr = mD3DDevice->CreatePixelShader(pPSBlob->GetBufferPointer(),
		pPSBlob->GetBufferSize(), NULL, &mPixelShader);

	if (FAILED(hr))
		return FALSE;

	const D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	hr = mD3DDevice->CreateInputLayout(layout, ARRAYSIZE(layout), pVSBlob->GetBufferPointer(),
		pVSBlob->GetBufferSize(), &mVertexLayout11);

	if (FAILED(hr))
		return FALSE;

	SafeRelease(pVSBlob);
	SafeRelease(pPSBlob);

	D3D11_BUFFER_DESC Desc;
	Desc.Usage = D3D11_USAGE_DYNAMIC;
	Desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	Desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	Desc.MiscFlags = 0;

	Desc.ByteWidth = sizeof(CB_VS_PER_OBJECT);
	mD3DDevice->CreateBuffer(&Desc, NULL, &mVSPerObject);
	Desc.ByteWidth = sizeof(CB_PS_PER_OBJECT);
	mD3DDevice->CreateBuffer(&Desc, NULL, &mPSPerObject);
	Desc.ByteWidth = sizeof(CB_PS_PER_FRAME);
	mD3DDevice->CreateBuffer(&Desc, NULL, &mPSPerFrame);

	return TRUE;
}
