#pragma once
#include "PostProcessor.h"

struct BillboardConstBuffer
{
	D3DXMATRIX mProjection;
};

class Billboard
{
public:
	Billboard();
	virtual ~Billboard();

	BOOL	Init();
	void	Render();

private:

	BOOL	CompileShader();
	BOOL	CreateQuad();
	BOOL	CreateConstBuffer();
	HRESULT	CompileShaderFromFile(WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut);

	D3DXMATRIX				mWorld;

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

	// get from renderer
	ID3D11Device*           mD3DDevice = NULL;
	ID3D11DeviceContext*    mD3DDeviceContext = NULL;

	// get last error
	HRESULT hr = S_OK;
};

