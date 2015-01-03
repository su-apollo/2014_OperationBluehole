#pragma once
#include "PostProcessor.h"

struct BillboardConstBuffer
{
	D3DXMATRIX mWorldViewProjection[2];
};

class Billboard
{
public:
	Billboard();
	virtual ~Billboard();

	BOOL					Init(const LPCWSTR path);
	void					Render(D3DXVECTOR4 pos);
	void					RenderInstanced(UINT num, D3DXVECTOR4* pos);

private:

	void					SetRenderState(UINT num, D3DXVECTOR4* pos);

	BOOL					CompileShader();
	BOOL					CreateQuad();
	BOOL					CreateConstBuffer();
	HRESULT					CompileShaderFromFile(WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut);
	BOOL					LoadTexture(const LPCWSTR path);

	ID3D11VertexShader*     mVertexShader = NULL;
	ID3D11Buffer*			mVSConstBuffer = NULL;
	ID3D11PixelShader*		mPixelShader = NULL;

	ID3D11Buffer*           mVertexBuffer = NULL;
	ID3D11Buffer*           mIndexBuffer = NULL;

	ID3D11InputLayout*      mVertexLayout11 = NULL;

	WCHAR*					mVertexShaderPath = L"BillBoardVertexShader.hlsl";
	LPCSTR					mVertexShaderMain = "main";
	LPCSTR					mVertexShaderModel = "vs_5_0";

	WCHAR*					mPixelShaderPath = L"BillBoardPixelShader.hlsl";
	LPCSTR					mPixelShaderMain = "main";
	LPCSTR					mPixelShaderModel = "ps_5_0";

	ID3D11ShaderResourceView*	mTexture = NULL;

	// get from renderer
	ID3D11Device*           mD3DDevice = NULL;
	ID3D11DeviceContext*    mD3DDeviceContext = NULL;

	// get last error
	HRESULT hr = S_OK;
};

