#pragma once
#include "Singleton.h"
#include "RenderTarget.h"

#define RENDER_TARGET_NUM 3


// todo : 화면크기가 변경될 경우
class RTManager : public Singleton<RTManager>
{
public:
	RTManager();
	~RTManager();

	BOOL Init();
	BOOL CreateRenderTargets();
	void ClearRenderTargets();
	void SetRenderTargetToGBuff();

	ID3D11ShaderResourceView* GetNormalTexRV() { return mNormalsBuff.GetShaderResourceView(); }
	ID3D11ShaderResourceView* GetDiffuseTexRV() { return mDiffuseBuff.GetShaderResourceView(); }
	ID3D11ShaderResourceView* GetSpecularTexRV() { return mSpecularBuff.GetShaderResourceView(); }
	ID3D11ShaderResourceView* GetDepthTexRV() { return mDepthBuff.GetShaderResourceView(); }

private:

	void GetWindowSize(HWND hWnd);

	// render targets
	RenderTarget			mNormalsBuff;
	RenderTarget			mDiffuseBuff;
	RenderTarget			mSpecularBuff;
	DepthRenderTarget		mDepthBuff;

	// get from renderer
	UINT					mWinWidth = 0;
	UINT					mWinHeight = 0;
	ID3D11Device*			mD3DDevice = NULL;
	ID3D11DeviceContext*	mD3DDeviceContext = NULL;
};

