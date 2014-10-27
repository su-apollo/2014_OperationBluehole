#include "stdafx.h"
#include "RenderStateManager.h"
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
		//desc.DepthFunc = D3D11_COMPARISON_GREATER_EQUAL;
		desc.DepthFunc = D3D11_COMPARISON_LESS;
		mD3DDevice->CreateDepthStencilState(&desc, &mDepthState);
	}

	{
		CD3D11_DEPTH_STENCIL_DESC desc(
			TRUE, D3D11_DEPTH_WRITE_MASK_ZERO, D3D11_COMPARISON_GREATER_EQUAL,    // Depth
			TRUE, 0xFF, 0xFF,                                                     // Stencil
			D3D11_STENCIL_OP_KEEP, D3D11_STENCIL_OP_KEEP, D3D11_STENCIL_OP_KEEP, D3D11_COMPARISON_EQUAL, // Front face stencil
			D3D11_STENCIL_OP_KEEP, D3D11_STENCIL_OP_KEEP, D3D11_STENCIL_OP_KEEP, D3D11_COMPARISON_EQUAL  // Back face stencil
			);
		mD3DDevice->CreateDepthStencilState(&desc, &mEqualStencilState);
	}

	{
		CD3D11_BLEND_DESC desc(D3D11_DEFAULT);
		mD3DDevice->CreateBlendState(&desc, &mGeometryBlendState);
	}

	{
		CD3D11_BLEND_DESC desc(D3D11_DEFAULT);
		// Additive blending
		desc.RenderTarget[0].BlendEnable = true;
		desc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
		desc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
		desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
		desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		mD3DDevice->CreateBlendState(&desc, &mLightingBlendState);
	}
}
