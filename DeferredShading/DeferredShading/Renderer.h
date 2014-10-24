#pragma once
#include "Singleton.h"
#include "Camera.h"
#include "RenderObj.h"
#include "RenderTarget.h"
#include "Elin.h"

// just draw single obj
class Renderer : public Singleton<Renderer>
{
public:
	Renderer();
	~Renderer();

	BOOL Init();

	void Render();
	void ClearBackBuff();
	void ClearDepthStencilBuff();
	void SetupViewPort();
	void SetRasterizeStage();
	void SwapChain() { mSwapChain->Present(0, 0); }

	ID3D11Device*				GetDevice() { return mD3DDevice; }
	ID3D11DeviceContext*		GetDeviceContext() { return mD3DDeviceContext; }
	ID3D11DepthStencilView*		GetDepthStencilView() { return mDepthStencilView; }
	ID3D11RenderTargetView*		GetRenderTargetView() { return mRenderTargetView; }
	ID3D11ShaderResourceView*	GetDepthStencilRV() { return mDepthStencilRV; }

private:

	void GetWindowSize(HWND hWnd);

	BOOL CreateDevice(HWND hWnd);
	BOOL CreateRasterizeState();
	BOOL CreateDepthStencilBuffer();

	void DestroyDevice();

	D3D_DRIVER_TYPE				mDriverType = D3D_DRIVER_TYPE_NULL;
	D3D_FEATURE_LEVEL			mFeatureLevel = D3D_FEATURE_LEVEL_11_0;

	// render 된 결과물이 backbuffer에 쓰기전 하나 이상의 surface에 저장
	IDXGISwapChain*				mSwapChain = NULL;

	ID3D11Device*				mD3DDevice = NULL;
	ID3D11DeviceContext*		mD3DDeviceContext = NULL;

	ID3D11RasterizerState*		mRasterizerState = NULL;

	ID3D11RenderTargetView*		mRenderTargetView = NULL;
	ID3D11DepthStencilView*		mDepthStencilView = NULL;
	ID3D11ShaderResourceView*	mDepthStencilRV = NULL;

	UINT					mDPCallNum = 0;

	UINT					mWinWidth = 0;
	UINT					mWinHeight = 0;

	// contents
	Elin					mCube;
	//RenderObj				mCube;

	// get last error
	HRESULT hr = S_OK;
};

