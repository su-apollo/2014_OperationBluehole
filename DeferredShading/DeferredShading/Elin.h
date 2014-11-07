#pragma once

#include <fbxsdk.h>
#include <xnamath.h>
#include "Singleton.h"
#include "ModelManager.h"

static const LPCWSTR	ELIN_TEXTURE_FACE_DIFF = L"ElinModel/Popori_F_Face01_diff.bmp";
static const LPCWSTR	ELIN_TEXTURE_FACE_NORM = L"ElinModel/Popori_F_Face01_norm.bmp";
static const LPCWSTR	ELIN_TEXTURE_FACE_SPEC = L"ElinModel/Popori_F_Face01_spec.bmp";

static const LPCWSTR	ELIN_TEXTURE_BODY_DIFF = L"ElinModel/Popori_F_H00_Body_diff.bmp";
static const LPCWSTR	ELIN_TEXTURE_BODY_NORM = L"ElinModel/Popori_F_H00_Body_norm.bmp";
static const LPCWSTR	ELIN_TEXTURE_BODY_SPEC = L"ElinModel/Popori_F_H00_Body_spec.bmp";

static const LPCWSTR	ELIN_TEXTURE_HAND_DIFF = L"ElinModel/Popori_F_H00_Hand_diff.bmp";
static const LPCWSTR	ELIN_TEXTURE_HAND_NORM = L"ElinModel/Popori_F_H00_Hand_norm.bmp";
static const LPCWSTR	ELIN_TEXTURE_HAND_SPEC = L"ElinModel/Popori_F_H00_Hand_spec.bmp";

static const LPCWSTR	ELIN_TEXTURE_LEG_DIFF = L"ElinModel/Popori_F_H00_Leg_diff.bmp";
static const LPCWSTR	ELIN_TEXTURE_LEG_NORM = L"ElinModel/Popori_F_H00_Leg_norm.bmp";
static const LPCWSTR	ELIN_TEXTURE_LEG_SPEC = L"ElinModel/Popori_F_H00_Leg_spec.bmp";

static const LPCWSTR	ELIN_TEXTURE_HAIR_DIFF = L"ElinModel/popori_F_hair07_diff.bmp";
static const LPCWSTR	ELIN_TEXTURE_HAIR_NORM = L"ElinModel/popori_F_hair07_norm.bmp";
static const LPCWSTR	ELIN_TEXTURE_HAIR_SPEC = L"ElinModel/popori_F_hair07_spec.bmp";

static const CHAR* ELIN_PATH = "ElinModel/Popori_F_H00_dance_good.FBX";

static const WCHAR* ELIN_VS_PATH = L"ElinVertexShader.hlsl";
static const LPCSTR	ELIN_VS_MAIN = "main";
static const LPCSTR ELIN_VS_MODEL = "vs_5_0";

static const WCHAR* ELIN_PS_PATH = L"ElinPixelShader.hlsl";
static const LPCSTR	ELIN_PS_MAIN = "main";
static const LPCSTR ELIN_PS_MODEL = "ps_5_0";

struct VSConstantBuffer
{
	D3DXMATRIX mWorld;
	D3DXMATRIX mView;
	D3DXMATRIX mProjection;
};

struct MeshData
{
	ID3D11Buffer*           mVertexBuffer = NULL;
	ID3D11Buffer*           mIndexBuffer = NULL;

	int						mNumIndex = 0;

	ID3D11ShaderResourceView*	mTextureRVDiff = NULL;
	ID3D11ShaderResourceView*	mTextureRVNorm = NULL;
	ID3D11ShaderResourceView*	mTextureRVSpec = NULL;

	D3DXMATRIX				mWorld;
};

typedef std::shared_ptr<MeshData> MeshDataPointer;

class Elin
{
public:
	Elin();
	virtual ~Elin();

	BOOL Init();
	void Render();
	void Release();
	void CleanUp();

	void IsRotate();

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

	BOOL					mIsRotating = TRUE;
};

