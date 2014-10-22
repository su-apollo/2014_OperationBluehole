#include "stdafx.h"
#include "GBuffManager.h"
#include "Renderer.h"
#include "App.h"

GBuffManager::GBuffManager()
{
}


GBuffManager::~GBuffManager()
{
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

	float ClearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	for (int i = 0; i < 2; ++i)
		mD3DDeviceContext->ClearRenderTargetView(renderTargets[i], ClearColor);

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



