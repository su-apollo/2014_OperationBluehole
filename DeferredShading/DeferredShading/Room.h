#pragma once

#include <fbxsdk.h>
#include <xnamath.h>
#include "Singleton.h"
#include "ModelManager.h"
#include "Elin.h"

static const CHAR*		ROOM_PATH = "bg/BG_ColorBleedTest00_SM.FBX";

static const LPCWSTR	ROOM_TEXTURE_DIFF = L"bg/BG_ColorBleedTest00_diff.bmp";
static const LPCWSTR	ROOM_TEXTURE_NORM = L"bg/BG_ColorBleedTest00_norm.bmp";

static const LPCWSTR	ROOM_ITEM_TEXTURE_DIFF = L"bg/BG_ColorBleedTest01_diff.bmp";
static const LPCWSTR	ROOM_ITEM_TEXTURE_NORM = L"bg/BG_ColorBleedTest01_norm.bmp";

static const WCHAR*		ROOM_VS_PATH = L"ElinVertexShader.hlsl";
static const LPCSTR		ROOM_VS_MAIN = "main";
static const LPCSTR		ROOM_VS_MODEL = "vs_5_0";

static const WCHAR*		ROOM_PS_PATH = L"ElinPixelShader.hlsl";
static const LPCSTR		ROOM_PS_MAIN = "main";
static const LPCSTR		ROOM_PS_MODEL = "ps_5_0";

class Room
{
public:
	Room();
	virtual ~Room();

	BOOL Init();
	void Render();
	void Release();
	void CleanUp();

private:

	void GetModelMeshData();
	void GetMeshData(FbxNode* inNode);

	BOOL CompileShader();
	BOOL	CompileVertexShader();
	BOOL	CompilePixelShader();
	HRESULT CompileShaderFromFile(WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut);

	BOOL	CreateModelBuffer();
	BOOL	CreateMeshBuffer(MeshPointer mesh);
	BOOL	CreateMeshVB(MeshPointer mesh, MeshDataPointer meshData);
	BOOL	CreateMeshIB(MeshPointer mesh, MeshDataPointer meshData);
	BOOL	CreateMeshCB(MeshPointer mesh, MeshDataPointer meshData);

	BOOL LoadTexture();
	BOOL LoadMeshTexture(MeshPointer mesh, MeshDataPointer meshData);


	void RenderMesh(MeshDataPointer meshData);

	ID3D11VertexShader*     mVertexShader = NULL;
	ID3D11PixelShader*      mPixelShader = NULL;

	ID3D11InputLayout*      mVertexLayout11 = NULL;

	ID3D11Buffer*           mVSConstBuffer = NULL;
	ID3D11Buffer*			mPSConstBuffer = NULL;

	D3DXMATRIX				mWorld;

	// get to Renderer
	ID3D11Device*           mD3DDevice = NULL;
	ID3D11DeviceContext*    mD3DDeviceContext = NULL;

	// get last error
	HRESULT hr = S_OK;

	FbxScene* mFbxScene = nullptr;

	//여러 매쉬를 포함하고 있는 전체 모델
	std::vector<MeshPointer> mModel;
	std::vector <MeshDataPointer>	mMeshData;
};

