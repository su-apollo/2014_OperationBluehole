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
	
	if (!mCube.FillBuffer())
	{
		MessageBox(App::GetInstance()->GetHandleMainWindow(), L"FillBuffer Error!", L"Error!", MB_ICONINFORMATION | MB_OK);
		DestroyDevice();
		return FALSE;
	}

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
	
	mCamera.Update();

	//update constbuff
	D3DXMATRIX matWorld;
	D3DXMATRIX matView;
	D3DXMATRIX matProj;
	ConstantBuffer cb;
	D3DXMatrixTranspose( &matWorld, &(mCube.GetMatWorld()));
	D3DXMatrixTranspose(&matView, &(mCamera.GetMatView()));
	D3DXMatrixTranspose(&matProj, &(mCamera.GetMatProj()));
	cb.mWorld = matWorld;
	cb.mView = matView;
	cb.mProjection = matProj;
	mD3DDeviceContext->UpdateSubresource(mConstantBuffer, 0, NULL, &cb, 0, 0);
	
	//draw
	mD3DDeviceContext->VSSetShader(mVertexShader, NULL, 0);
	mD3DDeviceContext->VSSetConstantBuffers(0, 1, &mConstantBuffer);
	mD3DDeviceContext->PSSetShader(mPixelShader, NULL, 0);
	mD3DDeviceContext->DrawIndexed(36, 0, 0);

	// backbuffer to front
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
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	UINT numElements = ARRAYSIZE(layout);

	hr = mD3DDevice->CreateInputLayout(layout, numElements, pVSBlob->GetBufferPointer(),
		pVSBlob->GetBufferSize(), &mVertexLayout11);

	SafeRelease(pVSBlob);

	if (FAILED(hr))
		return FALSE;

	mD3DDeviceContext->IASetInputLayout(mVertexLayout11);

	ID3DBlob* pPSBlob = NULL;
	hr = CompileShaderFromFile(const_cast<WCHAR*>(PS_PATH), PS_MAIN, "ps_4_0_level_9_1", &pPSBlob);
	if (FAILED(hr))
		return FALSE;
		
	hr = mD3DDevice->CreatePixelShader(pPSBlob->GetBufferPointer(),
		pPSBlob->GetBufferSize(), NULL, &mPixelShader);

	SafeRelease(pPSBlob);
	if (FAILED(hr))
		return FALSE;

	D3D11_BUFFER_DESC bd;
	//아래 zeromemory를 꼭해야함
	ZeroMemory(&bd, sizeof(bd));
	// Create the constant buffer
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(ConstantBuffer);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	hr = mD3DDevice->CreateBuffer(&bd, NULL, &mConstantBuffer);

	if (FAILED(hr))
		return FALSE;

	return TRUE;
}
