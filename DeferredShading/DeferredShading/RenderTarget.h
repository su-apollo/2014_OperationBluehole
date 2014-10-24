#pragma once







class RenderTarget
{
public:
	RenderTarget();
	~RenderTarget();


	BOOL Init(UINT width, UINT height, DXGI_FORMAT format);

	ID3D11RenderTargetView*		GetRenderTargetView(){ return mRenderTargetView; }
	ID3D11ShaderResourceView*	GetTextureRV() { return mTextureRV; }

private:

	ID3D11Device*				mD3DDevice = NULL;
	ID3D11DeviceContext*		mD3DDeviceContext = NULL;

	UINT mWidth;
	UINT mHeight;

	ID3D11RenderTargetView*		mRenderTargetView = NULL;
	ID3D11ShaderResourceView*	mTextureRV = NULL;


	// get last error
	HRESULT hr = S_OK;

};

