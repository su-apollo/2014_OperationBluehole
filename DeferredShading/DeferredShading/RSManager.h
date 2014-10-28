#pragma once
#include "Singleton.h"






class RenderStateManager : public Singleton<RenderStateManager>
{
public:
	RenderStateManager();
	~RenderStateManager();

	void Init();

	ID3D11DepthStencilState*	GetDepthState() { return mDepthState; }

private:

	ID3D11Device*				mD3DDevice = NULL;
	ID3D11DeviceContext*		mD3DDeviceContext = NULL;

	ID3D11DepthStencilState*	mDepthState = NULL;
};

