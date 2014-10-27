#pragma once
#include "Singleton.h"






class RenderStateManager : public Singleton<RenderStateManager>
{
public:
	RenderStateManager();
	~RenderStateManager();

	void Init();

	ID3D11DepthStencilState*	GetDepthState() { return mDepthState; }
	ID3D11DepthStencilState*	GetEqualStencilState() { return mEqualStencilState; }

	ID3D11BlendState*			GetGeometryBlendState() { return mGeometryBlendState; }
	ID3D11BlendState*			GetLightingBlendState() { return mLightingBlendState; }

private:

	ID3D11Device*				mD3DDevice = NULL;
	ID3D11DeviceContext*		mD3DDeviceContext = NULL;

	ID3D11DepthStencilState*	mDepthState = NULL;
	ID3D11DepthStencilState*	mEqualStencilState = NULL;

	ID3D11BlendState*			mGeometryBlendState = NULL;
	ID3D11BlendState*			mLightingBlendState = NULL;
};

