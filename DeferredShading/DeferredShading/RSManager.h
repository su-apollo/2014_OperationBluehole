#pragma once
#include "Singleton.h"






class RenderStateManager : public Singleton<RenderStateManager>
{
public:
	RenderStateManager();
	~RenderStateManager();

	void Init();
	void SetUseAlpha();
	void SetBlendStateDefault();

	ID3D11DepthStencilState*	GetDepthState() { return mDepthState; }
	ID3D11BlendState*			GetBlendAlpha() { return mBlendAlpha; }
	ID3D11BlendState*			GetBlendDefault() { return mBlendDefault; }

private:

	ID3D11Device*				mD3DDevice = NULL;
	ID3D11DeviceContext*		mD3DDeviceContext = NULL;

	ID3D11DepthStencilState*	mDepthState = NULL;

	ID3D11BlendState*			mBlendAlpha = NULL;
	ID3D11BlendState*			mBlendDefault = NULL;
};

