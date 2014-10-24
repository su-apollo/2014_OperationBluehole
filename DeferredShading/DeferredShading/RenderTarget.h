#pragma once


class RenderTarget
{
public:
	RenderTarget();
	~RenderTarget();

	BOOL Init(UINT width, UINT height, DXGI_FORMAT format);

	ID3D11RenderTargetView*		GetRenderTargetView(){ return mRenderTargetView; }
	ID3D11ShaderResourceView*	GetShaderResourceView() { return mShaderResourceView; }

private:

	ID3D11Device*				mD3DDevice = NULL;
	ID3D11DeviceContext*		mD3DDeviceContext = NULL;

	UINT mWidth;
	UINT mHeight;

	ID3D11RenderTargetView*		mRenderTargetView = NULL;
	ID3D11ShaderResourceView*	mShaderResourceView = NULL;

	// get last error
	HRESULT hr = S_OK;

};

class DepthRenderTarget
{
public:
	DepthRenderTarget();
	~DepthRenderTarget();

	BOOL Init(UINT width, UINT height);

	ID3D11ShaderResourceView*	GetShaderResourceView() { return mShaderResourceView; }
	ID3D11DepthStencilView*		GetDepthStencilView() { return mDepthStencilView; }

private:

	ID3D11Device*				mD3DDevice = NULL;
	ID3D11DeviceContext*		mD3DDeviceContext = NULL;

	UINT mWidth;
	UINT mHeight;

	ID3D11DepthStencilView*		mDepthStencilView = NULL;
	ID3D11ShaderResourceView*	mShaderResourceView = NULL;

	// get last error
	HRESULT hr = S_OK;

};


