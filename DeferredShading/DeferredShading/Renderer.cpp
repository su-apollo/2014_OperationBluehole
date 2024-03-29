#include "stdafx.h"
#include "Renderer.h"
#include "App.h"
#include "Logger.h"
#include "RSManager.h"
#include "LightManager.h"

Renderer::Renderer()
{
}


Renderer::~Renderer()
{
	DestroyDevice();
}

BOOL Renderer::Init()
{
	HWND hWnd = App::GetInstance()->GetHandleMainWindow();

	if (!CreateDevice(hWnd))
	{
		MessageBox(hWnd, L"CreateDevice Error!", L"Error!", MB_ICONINFORMATION | MB_OK);
		DestroyDevice();
		return FALSE;
	}

	if (!mElin.Init())
	{
		MessageBox(hWnd, L"mElin Init Error!", L"Error!", MB_ICONINFORMATION | MB_OK);
		DestroyDevice();
		return FALSE;
	}

	if (!mRoom.Init())
	{
		MessageBox(hWnd, L"mRoom Init Error!", L"Error!", MB_ICONINFORMATION | MB_OK);
		DestroyDevice();
		return FALSE;
	}

	return TRUE;
}

void Renderer::Render()
{
	// om - output merge
	mD3DDeviceContext->OMSetDepthStencilState(RenderStateManager::GetInstance()->GetDepthState(), 0);

	mElin.RenderAll();

	mRoom.RenderAll(mD3DDeviceContext);
}

BOOL Renderer::CreateDevice(HWND hWnd)
{
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
	sd.BufferDesc.Width = App::GetInstance()->GetWindowWidth();
	sd.BufferDesc.Height = App::GetInstance()->GetWindowHeight();
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

	return TRUE;
}

void Renderer::DestroyDevice()
{
	if (mD3DDeviceContext)
		mD3DDeviceContext->ClearState();

	SafeRelease(mRenderTargetView);
	SafeRelease(mSwapChain);
	SafeRelease(mD3DDeviceContext);
	SafeRelease(mD3DDevice);
}

void Renderer::SetupViewPort()
{
	// Setup the viewport
	D3D11_VIEWPORT vp;
	vp.Width = (FLOAT)App::GetInstance()->GetWindowWidth();
	vp.Height = (FLOAT)App::GetInstance()->GetWindowHeight();
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	mD3DDeviceContext->RSSetViewports(1, &vp);
}

void Renderer::ClearBackBuff()
{
	//clear
	float ClearColor[4] = { 0.0f, 0.125f, 0.3f, 1.0f };
	mD3DDeviceContext->ClearRenderTargetView(mRenderTargetView, ClearColor);
}


