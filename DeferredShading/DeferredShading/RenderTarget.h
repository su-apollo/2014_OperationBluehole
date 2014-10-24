#pragma once







class RenderTarget
{
public:
	RenderTarget();
	~RenderTarget();


	BOOL Init(UINT width, UINT height, DXGI_FORMAT format);

	ID3D11RenderTargetView*		GetRenderTargetView(){ return mRenderTargetView; }
	ID3D11ShaderResourceView*	GetTextureRV() { return mTextureRV; }
	ID3D11Texture2D*			GetTexture() { return mTexture; }

private:

	ID3D11Device*				mD3DDevice = NULL;
	ID3D11DeviceContext*		mD3DDeviceContext = NULL;

	UINT mWidth;
	UINT mHeight;

	ID3D11RenderTargetView*		mRenderTargetView = NULL;
	ID3D11ShaderResourceView*	mTextureRV = NULL;
	ID3D11Texture2D*			mTexture = NULL;


	// get last error
	HRESULT hr = S_OK;

};

