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
	HWND hWnd = App::GetInstance()->GetHandleMainWindow();

	if (!CreateRenderTargets())
	{
		MessageBox(hWnd, L"CreateGBuff Error!", L"Error!", MB_ICONINFORMATION | MB_OK);
		return FALSE;
	}

	return TRUE;
}

BOOL RTManager::CreateRenderTargets()
{
	UINT width = App::GetInstance()->GetWindowWidth();
	UINT height = App::GetInstance()->GetWindowHeight();

	if (!mNormalsBuff.Init(width, height, DXGI_FORMAT_R10G10B10A2_UNORM))
		return FALSE;

	if (!mDiffuseBuff.Init(width, height, DXGI_FORMAT_R8G8B8A8_UNORM_SRGB))
		return FALSE;

	if (!mSpecularBuff.Init(width, height, DXGI_FORMAT_R8G8B8A8_UNORM_SRGB))
		return FALSE;

	if (!mDepthBuff.Init(width, height))
		return FALSE;

	if (!mSDOBuff.Init(width, height, DXGI_FORMAT_R8G8B8A8_UNORM_SRGB))
		return FALSE;

	if (!mSceneBuff.Init(width, height, DXGI_FORMAT_R8G8B8A8_UNORM_SRGB))
		return FALSE;

	return TRUE;
}

void RTManager::ClearRenderTargets()
{
	ID3D11RenderTargetView* renderTargets[RENDER_TARGET_NUM] = { NULL };

	renderTargets[0] = mNormalsBuff.GetRenderTargetView();
	renderTargets[1] = mDiffuseBuff.GetRenderTargetView();
	renderTargets[2] = mSpecularBuff.GetRenderTargetView();

	float ClearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	for (int i = 0; i < RENDER_TARGET_NUM; ++i)
		mD3DDeviceContext->ClearRenderTargetView(renderTargets[i], ClearColor);

	mD3DDeviceContext->ClearDepthStencilView(mDepthBuff.GetDepthStencilView(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

void RTManager::SetRenderTargetToGBuff()
{
	ID3D11RenderTargetView* renderTargets[RENDER_TARGET_NUM] = { NULL };

	renderTargets[0] = mNormalsBuff.GetRenderTargetView();
	renderTargets[1] = mDiffuseBuff.GetRenderTargetView();
	renderTargets[2] = mSpecularBuff.GetRenderTargetView();

	mD3DDeviceContext->OMSetRenderTargets(3, renderTargets, mDepthBuff.GetDepthStencilView());
}

void RTManager::SetRenderTargetToSDOBuff()
{
	ID3D11RenderTargetView* renderTarget = mSDOBuff.GetRenderTargetView();
	mD3DDeviceContext->OMSetRenderTargets(1, &renderTarget, NULL);
}

void RTManager::SetRenderTargetToSceneBuff()
{
	ID3D11RenderTargetView* renderTarget = mSceneBuff.GetRenderTargetView();
	mD3DDeviceContext->OMSetRenderTargets(1, &renderTarget, NULL);
}






