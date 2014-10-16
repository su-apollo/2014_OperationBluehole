#pragma once
#include "Singleton.h"

enum GbufferType
{
	GBUFF_DEPTH = 0,
	GBUFF_NORMAL,
	GBUFF_ALBEDO,
	GBUFF_SPECULAR,
};

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



	UINT					mDPCallNum = 0;

};

