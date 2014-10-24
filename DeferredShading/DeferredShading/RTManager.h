#pragma once
#include "Singleton.h"
#include "RenderTarget.h"
#define GBUFFERNUM 3

// todo : 화면크기가 변경될 경우?
// todo : depthstendcil은 여기다 두는게 좋을까?
class RTManager : public Singleton<RTManager>
{
public:
	RTManager();
	~RTManager();

	BOOL Init();
	BOOL CreateGBuffers();
	void ClearRenderTargets();
	void SetRenderTargetToGBuff();

	ID3D11Texture2D* GetNormalTex() { return mNormalsBuff.GetTexture(); }
	ID3D11Texture2D* GetDiffuseTex() { return mDiffuseBuff.GetTexture(); }
	ID3D11Texture2D* GetSpecularTex() { return mSpecularBuff.GetTexture(); }
	ID3D11ShaderResourceView* GetNormalTexRV() { return mNormalsBuff.GetTextureRV(); }
	ID3D11ShaderResourceView* GetDiffuseTexRV() { return mDiffuseBuff.GetTextureRV(); }
	ID3D11ShaderResourceView* GetSpecularTexRV() { return mSpecularBuff.GetTextureRV(); }

private:

	void GetWindowSize(HWND hWnd);

	// render targets
	RenderTarget			mNormalsBuff;
	RenderTarget			mDiffuseBuff;
	RenderTarget			mSpecularBuff;

	// get from renderer
	UINT					mWinWidth = 0;
	UINT					mWinHeight = 0;
	ID3D11Device*			mD3DDevice = NULL;
	ID3D11DeviceContext*	mD3DDeviceContext = NULL;
	ID3D11DepthStencilView* mDepthStencilView = NULL;
};

