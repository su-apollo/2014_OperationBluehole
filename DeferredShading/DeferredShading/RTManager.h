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
	void SetRenderTargetToSDOBuff();
	void SetRenderTargetToSceneBuff();

	ID3D11ShaderResourceView* GetNormalTexRV() { return mNormalsBuff.GetShaderResourceView(); }
	ID3D11ShaderResourceView* GetDiffuseTexRV() { return mDiffuseBuff.GetShaderResourceView(); }
	ID3D11ShaderResourceView* GetSpecularTexRV() { return mSpecularBuff.GetShaderResourceView(); }
	ID3D11ShaderResourceView* GetDepthTexRV() { return mDepthBuff.GetShaderResourceView(); }

	ID3D11ShaderResourceView* GetSDOTexRV() { return mSDOBuff.GetShaderResourceView(); }
	ID3D11ShaderResourceView* GetSceneTexRV() { return mSceneBuff.GetShaderResourceView(); }

private:

	// render targets
	RenderTarget			mNormalsBuff;
	RenderTarget			mDiffuseBuff;
	RenderTarget			mSpecularBuff;
	DepthRenderTarget		mDepthBuff;

	RenderTarget			mSDOBuff;

	// buffer with out FXAA processing
	RenderTarget			mSceneBuff;

	// get from renderer
	ID3D11Device*			mD3DDevice = NULL;
	ID3D11DeviceContext*	mD3DDeviceContext = NULL;
};

