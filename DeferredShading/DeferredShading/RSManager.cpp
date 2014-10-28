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
}
