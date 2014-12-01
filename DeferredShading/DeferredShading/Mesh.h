#pragma once
#include "FBXLoader.h"

struct MESH_VERTEX
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

	ID3D11ShaderResourceView*	pSRVDiffuse = nullptr;
	ID3D11ShaderResourceView*	pSRVSpecular = nullptr;
	ID3D11ShaderResourceView*	pSRVNormal = nullptr;

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

//material 객체를 따로 빼서 정리해야할듯
//모양이 많이 안좋음
class Mesh
{
public:

	Mesh();
	virtual ~Mesh();

	//LoadFBX
	//CompileShader
	//CreateVSConstBuffer
	//순서로 호출해야함
	virtual BOOL Init() = 0;
	virtual void RenderAll(ID3D11DeviceContext* dc);

protected:

	virtual void	CleanUp(ID3D11DeviceContext* dc);

	// material은 하드코딩으로 구현해야함
	virtual BOOL	MaterialConstruction(ID3D11Device* device) = 0;

	void			RenderNode(ID3D11DeviceContext* dc, MESH_NODE& node);
	BOOL			LoadFBX(ID3D11Device* device, const char* path);

	BOOL			CreateNode(ID3D11Device* device);
	BOOL			VertexConstruction(ID3D11Device* device, FBX_MESH_NODE& fbxNode, MESH_NODE& meshNode);


	BOOL			CreateVertexBuffer(ID3D11Device* device, ID3D11Buffer** pBuffer, void* pVertices, uint32_t stride, uint32_t vertexCount);
	BOOL			CreateIndexBuffer(ID3D11Device* device, ID3D11Buffer** pBuffer, void* pIndices, uint32_t indexCount);
	
	BOOL			CompileShader(ID3D11Device* device, WCHAR* vpath, LPCSTR vmain, LPCSTR vmodel, WCHAR* ppath, LPCSTR pmain, LPCSTR pmodel);
	virtual BOOL	CompileVertexShader(ID3D11Device* device, WCHAR* path, LPCSTR main, LPCSTR model);
	BOOL			CompilePixelShader(ID3D11Device* device, WCHAR* path, LPCSTR main, LPCSTR model);
	HRESULT			CompileShaderFromFile(WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut);

	BOOL			CreateVSConstBuffer(ID3D11Device* device, UINT constBuffSize);

	D3DXMATRIX				mWorld;

	CFBXLoader*				mFBX;
	std::vector<MESH_NODE>	mMeshNodeArray;

	ID3D11VertexShader*     mVertexShader = NULL;
	ID3D11PixelShader*      mPixelShader = NULL;
	ID3D11InputLayout*      mVertexLayout11 = NULL;
	ID3D11Buffer*           mVSConstBuffer = NULL;
	ID3D11Buffer*			mPSConstBuffer = NULL;

	// get last error
	HRESULT hr = S_OK;

};

