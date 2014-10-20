#pragma once

static const WCHAR* VS_PATH = L"VertexShader.hlsl";
static const LPCSTR	VS_MAIN = "main";
static const LPCSTR VS_MODEL = "vs_4_0_level_9_1";

static const WCHAR* PS_PATH = L"PixelShader.hlsl";
static const LPCSTR	PS_MAIN = "main";
static const LPCSTR PS_MODEL = "ps_4_0_level_9_1";

static const LPCWSTR	TEXTURE_PATH = L"seafloor.dds";

static const int MAX_LIGHT = 2;

struct VSConstantBuffer
{
	D3DXMATRIX mWorld;
	D3DXMATRIX mView;
	D3DXMATRIX mProjection;
};

struct PSConstantBuffer
{
	D3DXVECTOR4 vLightDir[2];
	D3DXVECTOR4 vLightColor[2];
};

struct CubeVertex
{
	D3DXVECTOR3 pos;
	D3DXVECTOR3 Normal;
	D3DXVECTOR2	Tex;
};

//IA - Input Assembler Stage
//1. 입력버퍼 생성
//2. layout 생성(vertex 정의)
//3. IASetinputlayout, IASetbuffer를 통해서 IA단계에 생성 정보들을 전달
//4. IASetPrimitiveTopology를 통해 설정
//5. dpcall

// 지금은 큐브를 그리도록 되어있음
// todo : 상속받아서 쓸 수 있도록
// todo : 다수의 오브젝트를 모아서 찍도록(instancing 가능하도록)
class RenderObj
{
public:
	RenderObj();
	virtual ~RenderObj();

	D3DXMATRIX	GetMatWorld(){ return mWorld; }

	BOOL Init();
	void Release();
	BOOL CompileShader();
	BOOL CreateBuffer();

	void Render();

private:

	HRESULT CompileShaderFromFile(WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut);

	// todo : 아래 함수들 인자를 받아서 그릴 수 있도록 변경
	// todo : 인자는 마테리얼 객체에서 받아오도록
	// todo : 마테리얼 객체 생성, 마테리얼 객체의 갯수에 따라서 dp콜 호출
	BOOL	CompileVertexShader();
	BOOL	CompilePixelShader();

	// draw cube
	BOOL	CreateVertexBuff();
	BOOL	CreateIndexBuff();
	BOOL	CreateConstBuff();

	BOOL	LoadTexture();

	ID3D11VertexShader*     mVertexShader = NULL;
	ID3D11PixelShader*      mPixelShader = NULL;

	ID3D11InputLayout*      mVertexLayout11 = NULL;

	ID3D11Buffer*           mVertexBuffer = NULL;
	ID3D11Buffer*           mIndexBuffer = NULL;
	ID3D11Buffer*           mVSConstBuffer = NULL;
	ID3D11Buffer*			mPSConstBuffer = NULL;

	UINT					mVertexNum = 24;
	UINT					mIndexNum = 36;

	D3DXMATRIX				mWorld;

	ID3D11ShaderResourceView*	mTextureRV = NULL;
	ID3D11SamplerState*			mSamplerLinear = NULL;

	// get to Renderer
	ID3D11Device*           mD3DDevice = NULL;
	ID3D11DeviceContext*    mD3DDeviceContext = NULL;

	// get last error
	HRESULT hr = S_OK;
};


