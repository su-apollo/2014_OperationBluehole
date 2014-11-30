#pragma once
#include "FBXLoader.h"

struct ELIN_VERTEX
{
	D3DXVECTOR3 mPos;
	D3DXVECTOR3 mNormal;
	D3DXVECTOR3 mTangent;
	D3DXVECTOR2 mUV;
};

struct VS_CONSTBUFF_DATA
{
	D3DXMATRIX mWorld;
	D3DXMATRIX mView;
	D3DXMATRIX mProjection;
};

struct	VERTEX_DATA
{
	D3DXVECTOR3	vPos;
	D3DXVECTOR3	vNor;
	D3DXVECTOR3 vTan;
	D3DXVECTOR2	vTexcoord;
};

struct MATERIAL_DATA
{
	LPCWSTR	diffuseTexPath;
	LPCWSTR	specularTexPath;
	LPCWSTR	normalTexPath;

	ID3D11ShaderResourceView*	pSRVDiffuse;
	ID3D11ShaderResourceView*	pSRVSpecular;
	ID3D11ShaderResourceView*	pSRVNormal;

	void Release()
	{
		if (pSRVDiffuse)
		{
			pSRVDiffuse->Release();
			pSRVDiffuse = nullptr;
		}

		if (pSRVSpecular)
		{
			pSRVSpecular->Release();
			pSRVSpecular = nullptr;
		}

		if (pSRVNormal)
		{
			pSRVNormal->Release();
			pSRVNormal = nullptr;
		}
	}
};

// dpcall을 하는 단위
struct	MESH_NODE
{
	ID3D11Buffer*		m_pVB = nullptr;
	ID3D11Buffer*		m_pIB = nullptr;

	DWORD	vertexCount = 0;
	DWORD	indexCount = 0;

	MATERIAL_DATA materialData;

	float		mat4x4[16];
	// todo : D3DXMATRIX	mat;

	enum INDEX_BIT
	{
		INDEX_NOINDEX = 0,
		INDEX_16BIT,		
		INDEX_32BIT,		
	};
	INDEX_BIT	m_indexBit = INDEX_NOINDEX;

	void Release()
	{
		materialData.Release();

		if (m_pIB)
		{
			m_pIB->Release();
			m_pIB = nullptr;
		}

		if (m_pVB)
		{
			m_pVB->Release();
			m_pVB = nullptr;
		}
	}

	void SetIndexBit(const size_t indexCount)
	{
		m_indexBit = INDEX_NOINDEX;
		if (indexCount != 0)
			m_indexBit = INDEX_32BIT;
	};
};


class Elin
{
public:
	Elin();
	~Elin();

	BOOL Init();
	void RenderAll();
	void IsRotate();

private:

	void	RenderNode(MESH_NODE& node);
	BOOL	LoadFBX();

	BOOL	CreateNode();
	BOOL	VertexConstruction(FBX_MESH_NODE& fbxNode, MESH_NODE& meshNode);
	// material은 하드코딩으로 구현되어있음
	BOOL	MaterialConstruction();

	BOOL	CreateVertexBuffer(ID3D11Buffer** pBuffer, void* pVertices, uint32_t stride, uint32_t vertexCount);
	BOOL	CreateIndexBuffer(ID3D11Buffer** pBuffer, void* pIndices, uint32_t indexCount);
	BOOL	CreateMeshTexture(MESH_NODE& meshNode);

	BOOL	CompileShader();
	BOOL	CompileVertexShader();
	BOOL	CompilePixelShader();
	HRESULT CompileShaderFromFile(WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut);
	
	BOOL	CreateVSConstBuffer();	

	D3DXMATRIX				mWorld;

	CFBXLoader*				mFBX;
	std::vector<MESH_NODE>	mMeshNodeArray;

	ID3D11VertexShader*     mVertexShader = NULL;
	ID3D11PixelShader*      mPixelShader = NULL;
	ID3D11InputLayout*      mVertexLayout11 = NULL;
	ID3D11Buffer*           mVSConstBuffer = NULL;
	ID3D11Buffer*			mPSConstBuffer = NULL;

	const CHAR*		ELIN_PATH = "ElinModel/Popori_F_H00_dance_good.FBX";
	
	const WCHAR*	ELIN_VS_PATH = L"ElinVertexShader.hlsl";
	const LPCSTR	ELIN_VS_MAIN = "main";
	const LPCSTR	ELIN_VS_MODEL = "vs_5_0";

	const WCHAR*	ELIN_PS_PATH = L"ElinPixelShader.hlsl";
	const LPCSTR	ELIN_PS_MAIN = "main";
	const LPCSTR	ELIN_PS_MODEL = "ps_5_0";

	const LPCWSTR	ELIN_TEXTURE_FACE_DIFF = L"ElinModel/Popori_F_Face01_diff.bmp";
	const LPCWSTR	ELIN_TEXTURE_FACE_NORM = L"ElinModel/Popori_F_Face01_norm.bmp";
	const LPCWSTR	ELIN_TEXTURE_FACE_SPEC = L"ElinModel/Popori_F_Face01_spec.bmp";

	const LPCWSTR	ELIN_TEXTURE_BODY_DIFF = L"ElinModel/Popori_F_H00_Body_diff.bmp";
	const LPCWSTR	ELIN_TEXTURE_BODY_NORM = L"ElinModel/Popori_F_H00_Body_norm.bmp";
	const LPCWSTR	ELIN_TEXTURE_BODY_SPEC = L"ElinModel/Popori_F_H00_Body_spec.bmp";

	const LPCWSTR	ELIN_TEXTURE_HAND_DIFF = L"ElinModel/Popori_F_H00_Hand_diff.bmp";
	const LPCWSTR	ELIN_TEXTURE_HAND_NORM = L"ElinModel/Popori_F_H00_Hand_norm.bmp";
	const LPCWSTR	ELIN_TEXTURE_HAND_SPEC = L"ElinModel/Popori_F_H00_Hand_spec.bmp";

	const LPCWSTR	ELIN_TEXTURE_LEG_DIFF = L"ElinModel/Popori_F_H00_Leg_diff.bmp";
	const LPCWSTR	ELIN_TEXTURE_LEG_NORM = L"ElinModel/Popori_F_H00_Leg_norm.bmp";
	const LPCWSTR	ELIN_TEXTURE_LEG_SPEC = L"ElinModel/Popori_F_H00_Leg_spec.bmp";

	const LPCWSTR	ELIN_TEXTURE_HAIR_DIFF = L"ElinModel/popori_F_hair07_diff.bmp";
	const LPCWSTR	ELIN_TEXTURE_HAIR_NORM = L"ElinModel/popori_F_hair07_norm.bmp";
	const LPCWSTR	ELIN_TEXTURE_HAIR_SPEC = L"ElinModel/popori_F_hair07_spec.bmp";

	BOOL			mIsRotating = TRUE;

	// get to Renderer
	ID3D11Device*           mD3DDevice = NULL;
	ID3D11DeviceContext*    mD3DDeviceContext = NULL;

	// get last error
	HRESULT hr = S_OK;
};

