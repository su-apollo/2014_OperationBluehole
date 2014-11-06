#pragma once
#include "Singleton.h"
#include "Contents.h"
#include "Elin.h"

struct QuadVertex
{
	D3DXVECTOR3 Pos;
	D3DXVECTOR2 Tex;
};

struct PostProcessorConstantBuffer
{
	D3DXMATRIX	mInverseViewProj;
	D3DXMATRIX	mViewProj;
	D3DXMATRIX	mInverseProj;
	D3DXVECTOR4 vEye;
	D3DXVECTOR4 vLightPos[MAX_LIGHT];
	D3DXVECTOR4 vLightColor[MAX_LIGHT];
	D3DXVECTOR4	vLightRange[MAX_LIGHT];
	D3DXVECTOR4 vKernelVariables;
	D3DXVECTOR4 vSampleSphere[KERNEL_NUM];
};

static const LPCWSTR	NOISE_TEXTURE = L"noise_texture.bmp";

class PostProcessor : public Singleton<PostProcessor>
{
public:
	PostProcessor();
	~PostProcessor();

	BOOL					Init();

	void					Render();
	void					RenderCleanUp();
	void					ChangeKernelRadius(float radius);

private:

	BOOL					CompileShader();
	BOOL					CreateConstBuffer();
	BOOL					CreateQuad();
	BOOL					LoadNoiseTexture();

	HRESULT					CompileShaderFromFile(WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut);

	ID3D11VertexShader*     mVertexShader = NULL;
	ID3D11PixelShader*		mPixelShader = NULL;
	ID3D11Buffer*			mPSConstBuffer = NULL;

	ID3D11InputLayout*      mVertexLayout11 = NULL;

	// noise texture
	ID3D11ShaderResourceView*	mTextureNoise = NULL;

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

	// changeable varialbes
	float					mKernelRadius = 5;
	D3DXVECTOR4				mSampleSphere[KERNEL_NUM];
};

