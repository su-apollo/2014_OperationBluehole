#pragma once

static const WCHAR* VS_PATH = L"VertexShader.hlsl";
static const LPCSTR	VS_MAIN = "main";

static const WCHAR* PS_PATH = L"PixelShader.hlsl";
static const LPCSTR	PS_MAIN = "main";

struct ConstantBuffer
{
	D3DXMATRIX mWorld;
	D3DXMATRIX mView;
	D3DXMATRIX mProjection;
};

struct CubeVertex
{
	D3DXVECTOR3 pos;
	D3DXVECTOR4 color;
};

//IA - Input Assembler Stage
//1. �Է¹��� ����
//2. layout ����(vertex ����)
//3. IASetinputlayout, IASetbuffer�� ���ؼ� IA�ܰ迡 ���� �������� ����
//4. IASetPrimitiveTopology�� ���� ����
//5. dpcall
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
	BOOL	CompileVertexShader();
	BOOL	CompilePixelShader();

	// draw cube
	BOOL	CreateVertexBuff();
	BOOL	CreateIndexBuff();
	BOOL	CreateConstBuff();

	ID3D11VertexShader*     mVertexShader = NULL;
	ID3D11PixelShader*      mPixelShader = NULL;

	ID3D11InputLayout*      mVertexLayout11 = NULL;

	ID3D11Buffer*           mVertexBuffer = NULL;
	ID3D11Buffer*           mIndexBuffer = NULL;
	ID3D11Buffer*           mConstantBuffer = NULL;

	D3DXMATRIX				mWorld;

	// get to Renderer
	ID3D11Device*           mD3DDevice = NULL;
	ID3D11DeviceContext*    mD3DDeviceContext = NULL;

	// get last error
	HRESULT hr = S_OK;
};


