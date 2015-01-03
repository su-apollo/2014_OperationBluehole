#include "stdafx.h"
#include "RSManager.h"
#include "Renderer.h"

RenderStateManager::RenderStateManager()
{
}


RenderStateManager::~RenderStateManager()
{
}

void RenderStateManager::Init()
{
	mD3DDevice = Renderer::GetInstance()->GetDevice();
	mD3DDeviceContext = Renderer::GetInstance()->GetDeviceContext();

	{
		CD3D11_DEPTH_STENCIL_DESC desc(D3D11_DEFAULT);
		mD3DDevice->CreateDepthStencilState(&desc, &mDepthState);
	}

	{
		CD3D11_BLEND_DESC desc(D3D11_DEFAULT);
		desc.AlphaToCoverageEnable = TRUE;
		mD3DDevice->CreateBlendState(&desc, &mBlendAlpha);
	}

	{
		CD3D11_BLEND_DESC desc(D3D11_DEFAULT);
		mD3DDevice->CreateBlendState(&desc, &mBlendDefault);
	}
}

void RenderStateManager::SetUseAlpha()
{
	float blend_factor[4] = { 0.f, 0.f, 0.f, 0.f };
	mD3DDeviceContext->OMSetBlendState(mBlendAlpha, blend_factor, 0xffffffff);
}

void RenderStateManager::SetBlendStateDefault()
{
	float blend_factor[4] = { 0.f, 0.f, 0.f, 0.f };
	mD3DDeviceContext->OMSetBlendState(mBlendDefault, blend_factor, 0xffffffff);
}
