#pragma once

#include <fbxsdk.h>
#include <xnamath.h>
#include "Singleton.h"

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



struct Vertex
{
	D3DXVECTOR3 mPos;
	//D3DXVECTOR4 Color;
	D3DXVECTOR2 mUV;

	//D3DXVECTOR3 mNormal;

};

struct Mesh
{
	std::vector<Vertex> mVertex;
	std::vector<unsigned int> mIndices;
	int mNumPolygon;
	int mNumVertex;
	int mNumIndex;

	const WCHAR* mTexutreDiff;
	const WCHAR* mTexutreNorm;
	const WCHAR* mTexutreSpec;
};

struct Indices
{
	int i0, i1, i2;
};

struct MeshData
{
	ID3D11Buffer*           mVertexBuffer = NULL;
	ID3D11Buffer*           mIndexBuffer = NULL;

	int						mNumIndex = 0;

	ID3D11ShaderResourceView*	mTextureRVDiff = NULL;
	ID3D11ShaderResourceView*	mTextureRVNorm = NULL;
	ID3D11ShaderResourceView*	mTextureRVSpec = NULL;

	ID3D11SamplerState*			mSamplerLinear = NULL;
};

typedef std::shared_ptr<Mesh> EMesh;
typedef std::shared_ptr<MeshData> EMeshData;

class Elin
{
public:
	Elin();
	virtual ~Elin();

	BOOL Init();
	void Render();
	void Release();
	void CleanUp();

private:

	BOOL LoadFBX();
	void ProcessGeometry(FbxNode* inNode);

	BOOL CompileShader();
	BOOL	CompileVertexShader();
	BOOL	CompilePixelShader();
	HRESULT CompileShaderFromFile(WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut);

	BOOL	CreateModelBuffer();
	BOOL	CreateMeshBuffer(EMesh mesh);
	BOOL	CreateMeshVB(EMesh mesh, EMeshData meshData);
	BOOL	CreateMeshIB(EMesh mesh, EMeshData meshData);
	BOOL	CreateMeshCB(EMesh mesh, EMeshData meshData);

	BOOL LoadTexture();
	BOOL LoadMeshTexture(EMesh mesh, EMeshData meshData);

	std::string GetFileName(const char* fileName);

	void RenderMesh(EMeshData meshData);

	ID3D11VertexShader*     mVertexShader = NULL;
	ID3D11PixelShader*      mPixelShader = NULL;

	ID3D11InputLayout*      mVertexLayout11 = NULL;

	ID3D11Buffer*           mVSConstBuffer = NULL;
	ID3D11Buffer*			mPSConstBuffer = NULL;



	//ID3D11ShaderResourceView*	mTextureRV = NULL;
	//ID3D11SamplerState*			mSamplerLinear = NULL;

	D3DXMATRIX				mWorld;

	// get to Renderer
	ID3D11Device*           mD3DDevice = NULL;
	ID3D11DeviceContext*    mD3DDeviceContext = NULL;

	// get last error
	HRESULT hr = S_OK;

	FbxManager* mFbxManager = nullptr;
	FbxImporter* mImporter = nullptr;
	FbxScene* mFbxScene = nullptr;

	//여러 매쉬를 포함하고 있는 전체 모델
	std::vector<EMesh> mModel;

	std::vector <EMeshData>	mMeshData;


};

