#include "stdafx.h"
#include "RTManager.h"
#include "Renderer.h"
#include "App.h"

RTManager::RTManager()
{
}


RTManager::~RTManager()
{
}


BOOL RTManager::Init()
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

	return TRUE;
}

BOOL RTManager::CreateGBuffers()
{

	if (!mNormalsBuff.Init(mWinWidth, mWinHeight, DXGI_FORMAT_R10G10B10A2_UNORM))
		return FALSE;
		

	if (!mDiffuseBuff.Init(mWinWidth, mWinHeight, DXGI_FORMAT_R8G8B8A8_UNORM_SRGB))
		return FALSE;

	if (!mSpecularBuff.Init(mWinWidth, mWinHeight, DXGI_FORMAT_R8G8B8A8_UNORM_SRGB))
		return FALSE;

	return TRUE;
}

void RTManager::SetRenderTargetToGBuff()
{
	ID3D11RenderTargetView* renderTargets[GBUFFERNUM] = { NULL };

	renderTargets[0] = mNormalsBuff.GetRenderTargetView();
	renderTargets[1] = mDiffuseBuff.GetRenderTargetView();
	renderTargets[2] = mSpecularBuff.GetRenderTargetView();

	float ClearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	for (int i = 0; i < GBUFFERNUM; ++i)
		mD3DDeviceContext->ClearRenderTargetView(renderTargets[i], ClearColor);

	mD3DDeviceContext->OMSetRenderTargets(3, renderTargets, mDepthStencilView);
}

void RTManager::GetWindowSize(HWND hWnd)
{
	HRESULT hr = S_OK;

	RECT rc;
	GetClientRect(hWnd, &rc);
	mWinWidth = rc.right - rc.left;
	mWinHeight = rc.bottom - rc.top;
}



