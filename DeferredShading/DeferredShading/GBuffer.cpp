#include "stdafx.h"
#include "GBuffer.h"
#include "Renderer.h"



GBuffer::GBuffer()
{
}


GBuffer::~GBuffer()
{
	SafeRelease(mRenderTargetView);
	SafeRelease(mTexture);
}


BOOL GBuffer::Init(UINT width, UINT height, DXGI_FORMAT format)
{
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

	hr = mD3DDevice->CreateTexture2D(&desc, NULL, &mTexture);
	if (FAILED(hr))
		return FALSE;

	hr = mD3DDevice->CreateRenderTargetView(mTexture, NULL, &mRenderTargetView);
	if (FAILED(hr))
		return FALSE;

	hr = mD3DDevice->CreateShaderResourceView(mTexture, NULL, &mTextureRV);

	return TRUE;
}
