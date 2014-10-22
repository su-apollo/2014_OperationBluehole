#pragma once
#include "Singleton.h"
#include "GBuffer.h"





// todo : 화면크기가 변경될 경우?
// todo : depthstendcil은 여기다 두는게 좋을까?
class GBuffManager : public Singleton<GBuffManager>
{
public:
	GBuffManager();
	~GBuffManager();

	BOOL Init();
	BOOL CreateGBuffers();
	void ClearRenderTargets();
	void SetRenderTargetToGBuff();

	ID3D11Texture2D* GetNormalTex() { return mNormalsBuff.GetTexture(); }
	ID3D11Texture2D* GetAlbedoTex() { return mAlbedoBuff.GetTexture(); }
	ID3D11ShaderResourceView* GetNormalTexRV() { return mNormalsBuff.GetTextureRV(); }
	ID3D11ShaderResourceView* GetAlbedoTexRV() { return mAlbedoBuff.GetTextureRV(); }
	ID3D11ShaderResourceView* GetDepthTexRV(){ return mDepthStencilRV; }

private:

	void GetWindowSize(HWND hWnd);
	BOOL SetDepthStencilRV();

	// render targets
	GBuffer						mNormalsBuff;
	GBuffer						mAlbedoBuff;

	ID3D11ShaderResourceView*	mDepthStencilRV = NULL;

	// get from renderer
	UINT					mWinWidth = 0;
	UINT					mWinHeight = 0;
	ID3D11Device*			mD3DDevice = NULL;
	ID3D11DeviceContext*	mD3DDeviceContext = NULL;
	ID3D11DepthStencilView* mDepthStencilView = NULL;
};

