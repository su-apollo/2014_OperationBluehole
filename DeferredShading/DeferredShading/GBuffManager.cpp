#include "stdafx.h"
#include "GBuffManager.h"
#include "Renderer.h"
#include "App.h"

GBuffManager::GBuffManager()
{
}


GBuffManager::~GBuffManager()
{
	SafeRelease(mDepthStencilRV);
}


BOOL GBuffManager::Init()
{
	mD3DDevice = Renderer::GetInstance()->GetDevice();
	mD3DDeviceContext = Renderer::GetInstance()->GetDeviceContext();
	mDepthStencilView = Renderer::GetInstance()->GetDepthStencilView();
	HWND hWnd = App::GetInstance()->GetHandleMainWindow();
	GetWindowSize(hWnd);

	if (!CreateGBuffers())
	{
		MessageBox(hWnd, L"CreateGBuff Error!", L"Error!", MB_ICONINFORMATION | MB_OK);
		return FALSE;
	}

	if (!SetDepthStencilRV())
	{
		MessageBox(hWnd, L"SetDepthStencilRV Error!", L"Error!", MB_ICONINFORMATION | MB_OK);
		return FALSE;
	}

	return TRUE;
}

BOOL GBuffManager::CreateGBuffers()
{

	if (!mNormalsBuff.Init(mWinWidth, mWinHeight, DXGI_FORMAT_R10G10B10A2_UNORM))
		return FALSE;
		

	if (!mAlbedoBuff.Init(mWinWidth, mWinHeight, DXGI_FORMAT_R8G8B8A8_UNORM_SRGB))
		return FALSE;

	return TRUE;
}

void GBuffManager::SetRenderTargetToGBuff()
{
	ID3D11RenderTargetView* renderTargets[2] = { NULL };

	renderTargets[0] = mNormalsBuff.GetRenderTargetView();
	renderTargets[1] = mAlbedoBuff.GetRenderTargetView();

	// clear
	//float ClearColor[4] = { 0.0f, 0.3f, 0.0f, 1.0f };
	float ClearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	for (int i = 0; i < 2; ++i)
		mD3DDeviceContext->ClearRenderTargetView(renderTargets[i], ClearColor);
	mD3DDeviceContext->ClearDepthStencilView(mDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

	mD3DDeviceContext->OMSetRenderTargets(2, renderTargets, mDepthStencilView);
}

void GBuffManager::GetWindowSize(HWND hWnd)
{
	HRESULT hr = S_OK;

	RECT rc;
	GetClientRect(hWnd, &rc);
	mWinWidth = rc.right - rc.left;
	mWinHeight = rc.bottom - rc.top;
}

BOOL GBuffManager::SetDepthStencilRV()
{
	HRESULT hr = S_OK;

	D3D11_SHADER_RESOURCE_VIEW_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	desc.Texture2D.MipLevels = 1;
	desc.Texture2D.MostDetailedMip = 0;

	ID3D11Texture2D* depthStencil = Renderer::GetInstance()->GetDepthStencil();
	hr = mD3DDevice->CreateShaderResourceView(depthStencil, &desc, &mDepthStencilRV);

	if (FAILED(hr))
		return FALSE;

	return TRUE;
}



