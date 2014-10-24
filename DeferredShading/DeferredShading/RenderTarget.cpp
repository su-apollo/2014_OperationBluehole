#include "stdafx.h"
#include "RenderTarget.h"
#include "Renderer.h"



RenderTarget::RenderTarget()
{
}


RenderTarget::~RenderTarget()
{
	SafeRelease(mRenderTargetView);
	SafeRelease(mShaderResourceView);
}


BOOL RenderTarget::Init(UINT width, UINT height, DXGI_FORMAT format)
{
	ID3D11Texture2D* pTexture = NULL;

	mD3DDevice = Renderer::GetInstance()->GetDevice();
	mWidth = width;
	mHeight = height;

	D3D11_TEXTURE2D_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.Width = mWidth;
	desc.Height = mHeight;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = format;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;

	hr = mD3DDevice->CreateTexture2D(&desc, NULL, &pTexture);
	if (FAILED(hr))
		return FALSE;

	hr = mD3DDevice->CreateRenderTargetView(pTexture, NULL, &mRenderTargetView);
	if (FAILED(hr))
		return FALSE;

	hr = mD3DDevice->CreateShaderResourceView(pTexture, NULL, &mShaderResourceView);
	pTexture->Release();

	return TRUE;
}

DepthRenderTarget::DepthRenderTarget()
{

}

DepthRenderTarget::~DepthRenderTarget()
{
	SafeRelease(mDepthStencilView);
	SafeRelease(mShaderResourceView);
}

BOOL DepthRenderTarget::Init(UINT width, UINT height)
{
	mD3DDevice = Renderer::GetInstance()->GetDevice();
	mWidth = width;
	mHeight = height;

	ID3D11Texture2D* pDepthStencil = NULL;

	// Create depth stencil texture
	D3D11_TEXTURE2D_DESC descDepth;
	ZeroMemory(&descDepth, sizeof(descDepth));
	descDepth.Width = mWidth;
	descDepth.Height = mHeight;
	descDepth.MipLevels = 1;
	descDepth.ArraySize = 1;
	//descDepth.Format = DXGI_FORMAT_R24G8_TYPELESS;
	descDepth.Format = DXGI_FORMAT_R32_TYPELESS;
	descDepth.SampleDesc.Count = 1;
	descDepth.SampleDesc.Quality = 0;
	descDepth.Usage = D3D11_USAGE_DEFAULT;
	descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	descDepth.CPUAccessFlags = 0;
	descDepth.MiscFlags = 0;
	hr = mD3DDevice->CreateTexture2D(&descDepth, NULL, &pDepthStencil);
	if (FAILED(hr))
		return FALSE;

	// Create the depth stencil view
	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
	ZeroMemory(&descDSV, sizeof(descDSV));
	//descDSV.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	descDSV.Format = DXGI_FORMAT_D32_FLOAT;
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	descDSV.Texture2D.MipSlice = 0;
	hr = mD3DDevice->CreateDepthStencilView(pDepthStencil, &descDSV, &mDepthStencilView);
	if (FAILED(hr))
		return FALSE;

	D3D11_SHADER_RESOURCE_VIEW_DESC descSRV;
	ZeroMemory(&descSRV, sizeof(descSRV));
	//descSRV.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	descSRV.Format = DXGI_FORMAT_R32_FLOAT;
	descSRV.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	descSRV.Texture2D.MipLevels = 1;
	descSRV.Texture2D.MostDetailedMip = 0;

	hr = mD3DDevice->CreateShaderResourceView(pDepthStencil, &descSRV, &mShaderResourceView);
	pDepthStencil->Release();

	if (FAILED(hr))
		return FALSE;

	return TRUE;
}


