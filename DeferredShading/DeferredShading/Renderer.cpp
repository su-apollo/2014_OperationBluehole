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

	if (!mCube.Init())
	{
		MessageBox(App::GetInstance()->GetHandleMainWindow(), L"Cube Init Error!", L"Error!", MB_ICONINFORMATION | MB_OK);
		DestroyDevice();
		return FALSE;
	}

	return TRUE;
}

void Renderer::Render()
{
	//clear
	float ClearColor[4] = { 0.0f, 0.125f, 0.3f, 1.0f }; //red,green,blue,alpha
	mD3DDeviceContext->ClearRenderTargetView(mRenderTargetView, ClearColor);

	mCube.Render();

	// backbuffer to front
	mSwapChain->Present(0, 0);
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

	// Create depth stencil texture
	D3D11_TEXTURE2D_DESC descDepth;
	ZeroMemory(&descDepth, sizeof(descDepth));
	descDepth.Width = width;
	descDepth.Height = height;
	descDepth.MipLevels = 1;
	descDepth.ArraySize = 1;
	descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	descDepth.SampleDesc.Count = 1;
	descDepth.SampleDesc.Quality = 0;
	descDepth.Usage = D3D11_USAGE_DEFAULT;
	descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	descDepth.CPUAccessFlags = 0;
	descDepth.MiscFlags = 0;
	hr = mD3DDevice->CreateTexture2D(&descDepth, NULL, &mDepthStencil);
	if (FAILED(hr))
		return FALSE;

	// Create the depth stencil view
	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
	ZeroMemory(&descDSV, sizeof(descDSV));
	descDSV.Format = descDepth.Format;
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	descDSV.Texture2D.MipSlice = 0;
	hr = mD3DDevice->CreateDepthStencilView(mDepthStencil, &descDSV, &mDepthStencilView);
	if (FAILED(hr))
		return FALSE;

	// set rendertarget
	//mD3DDeviceContext->OMSetRenderTargets(1, &mRenderTargetView, mDepthStencilView);
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
	mCube.Release();

	if (mD3DDeviceContext)
		mD3DDeviceContext->ClearState();

	SafeRelease(mRenderTargetView);
	SafeRelease(mSwapChain);
	SafeRelease(mD3DDeviceContext);
	SafeRelease(mD3DDevice);
}


