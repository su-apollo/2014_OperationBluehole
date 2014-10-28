#pragma once
#include "Singleton.h"
#include "Elin.h"
#include "Config.h"

struct QuadVertex
{
	D3DXVECTOR3 Pos;
	D3DXVECTOR2 Tex;
};

struct PostProcessorConstantBuffer
{
	D3DXMATRIX	mInverseProj;
	D3DXVECTOR4 vEye;
	D3DXVECTOR4 vLightPos[MAX_LIGHT];
	D3DXVECTOR4 vLightColor[MAX_LIGHT];
	D3DXVECTOR4	vLightRange[MAX_LIGHT];
};

class PostProcessor : public Singleton<PostProcessor>
{
public:
	PostProcessor();
	~PostProcessor();

	BOOL					Init();

	void					Render();
	void					RenderCleanUp();

private:

	BOOL					CompileShader();
	BOOL					CreateConstBuffer();
	BOOL					CreateQuad();

	HRESULT					CompileShaderFromFile(WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut);

	ID3D11VertexShader*     mVertexShader = NULL;
	ID3D11PixelShader*		mPixelShader = NULL;
	ID3D11Buffer*			mPSConstBuffer = NULL;

	ID3D11InputLayout*      mVertexLayout11 = NULL;

	WCHAR*					mVertexShaderPath = L"Quad.hlsl";
	LPCSTR					mVertexShaderMain = "main";
	LPCSTR					mVertexShaderModel = "vs_4_0_level_9_3";

	WCHAR*					mPixelShaderPath = L"PostProcess.hlsl";
	LPCSTR					mPixelShaderMain = "main";
	LPCSTR					mPixelShaderModel = "ps_4_0_level_9_3";

	// draw quad
	ID3D11Buffer*           mVertexBuffer = NULL;
	ID3D11Buffer*           mIndexBuffer = NULL;

	// get from renderer
	ID3D11Device*           mD3DDevice = NULL;
	ID3D11DeviceContext*    mD3DDeviceContext = NULL;
	ID3D11RenderTargetView* mBackBuffRTV = NULL;

	// get last error
	HRESULT hr = S_OK;
};

