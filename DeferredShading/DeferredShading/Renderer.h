#pragma once
#include "Singleton.h"
#include "Camera.h"
#include "RenderObj.h"

// just draw single obj
class Renderer : public Singleton<Renderer>
{
public:
	Renderer();
	~Renderer();

	BOOL Init();

	void Render();

	ID3D11Device* GetDevice() { return mD3DDevice; }
	ID3D11DeviceContext* GetDeviceContext() { return mD3DDeviceContext; }

private:

	BOOL CreateDevice(HWND hWnd);
	void DestroyDevice();

	D3D_DRIVER_TYPE			mDriverType = D3D_DRIVER_TYPE_NULL;
	D3D_FEATURE_LEVEL       mFeatureLevel = D3D_FEATURE_LEVEL_11_0;

	// render 된 결과물이 backbuffer에 쓰기전 하나 이상의 surface에 저장
	IDXGISwapChain*			mSwapChain = NULL;

	ID3D11Device*           mD3DDevice = NULL;
	ID3D11DeviceContext*    mD3DDeviceContext = NULL;

	ID3D11RenderTargetView* mRenderTargetView = NULL;

	UINT					mDPCallNum = 0;

	// contents
	RenderObj				mCube;
};

