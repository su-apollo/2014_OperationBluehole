#pragma once
#include "Singleton.h"

enum GbufferType
{
	GBUFF_DEPTH = 0,
	GBUFF_NORMAL,
	GBUFF_ALBEDO,
	GBUFF_SPECULAR,
};

struct CB_VS_PER_OBJECT
{
	D3DXMATRIX mWorldViewProj;
	D3DXMATRIX mWorld;
};
UINT gCBVSPerObjectBind = 0;

struct CB_PS_PER_OBJECT
{
	D3DXVECTOR4 mObjectColor;
};
UINT gCBPSPerObjectBind = 0;

struct CB_PS_PER_FRAME
{
	D3DXVECTOR4 mLightDirAmbient;
};
UINT gCBPSPerFrameBind = 1;


static const WCHAR* VS_PATH = L"VertexShader.hlsl";
static const LPCSTR	VS_MAIN = "main";

static const WCHAR* PS_PATH = L"PixelShader.hlsl";
static const LPCSTR	PS_MAIN = "main";

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

	BOOL CompileShader();

	HRESULT CompileShaderFromFile(WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut);

	D3D_DRIVER_TYPE			mDriverType = D3D_DRIVER_TYPE_NULL;
	D3D_FEATURE_LEVEL       mFeatureLevel = D3D_FEATURE_LEVEL_11_0;

	// render 된 결과물이 backbuffer에 쓰기전 하나 이상의 surface에 저장
	IDXGISwapChain*			mSwapChain = NULL;

	ID3D11Device*           mD3DDevice = NULL;
	ID3D11DeviceContext*    mD3DDeviceContext = NULL;

	ID3D11RenderTargetView* mRenderTargetView = NULL;

	ID3D11VertexShader*     mVertexShader = NULL;
	ID3D11PixelShader*      mPixelShader = NULL;

	ID3D11InputLayout*      mVertexLayout11 = NULL;

	// shader content
	ID3D11Buffer*           mVSPerObject = NULL;
	ID3D11Buffer*           mPSPerObject = NULL;
	ID3D11Buffer*           mPSPerFrame = NULL;

	UINT					mDPCallNum = 0;

};

