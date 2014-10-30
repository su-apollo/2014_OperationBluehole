#pragma once
#include "FBXLoader.h"

struct	VERTEX_DATA
{
	D3DXVECTOR4	vPos;
	D3DXVECTOR4	vNor;
	D3DXVECTOR2	vTexcoord;
};

struct MATERIAL_CONSTANT_DATA
{
	D3DXVECTOR4	ambient;
	D3DXVECTOR4	diffuse;
	D3DXVECTOR4	specular;
	D3DXVECTOR4	emmisive;
};

struct MATERIAL_DATA
{
	D3DXVECTOR4	ambient;
	D3DXVECTOR4	diffuse;
	D3DXVECTOR4	specular;
	D3DXVECTOR4	emmisive;
	float specularPower;
	float TransparencyFactor;

	MATERIAL_CONSTANT_DATA materialConstantData;

	ID3D11ShaderResourceView*	pSRV;
	ID3D11SamplerState*         pSampler;
	ID3D11Buffer*				pMaterialCb;

	MATERIAL_DATA()
	{
		pSRV = nullptr;
		pSampler = nullptr;
		pMaterialCb = nullptr;
	}

	void Release()
	{
		if (pMaterialCb)
		{
			pMaterialCb->Release();
			pMaterialCb = nullptr;
		}

		if (pSRV)
		{
			pSRV->Release();
			pSRV = nullptr;
		}

		if (pSampler)
		{
			pSampler->Release();
			pSampler = nullptr;
		}
	}
};

struct	MESH_NODE
{
	ID3D11Buffer*		m_pVB;
	ID3D11Buffer*		m_pIB;
	ID3D11InputLayout*	m_pInputLayout;

	DWORD	vertexCount;
	DWORD	indexCount;

	MATERIAL_DATA materialData;

	float	mat4x4[16];

	// INDEX BUFFER‚ÌBIT
	enum INDEX_BIT
	{
		INDEX_NOINDEX = 0,
		INDEX_16BIT,	
		INDEX_32BIT,
	};
	INDEX_BIT	m_indexBit;

	MESH_NODE()
	{
		m_pVB = nullptr;
		m_pIB = nullptr;
		m_pInputLayout = nullptr;
		m_indexBit = INDEX_NOINDEX;
		vertexCount = 0;
		indexCount = 0;
	}

	void Release()
	{
		materialData.Release();

		if (m_pInputLayout)
		{
			m_pInputLayout->Release();
			m_pInputLayout = nullptr;
		}
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

class FBXRenderer
{
public:
	FBXRenderer();
	~FBXRenderer();

	void Release();

	HRESULT LoadFBX(const char* filename, ID3D11Device*	pd3dDevice);
	HRESULT CreateInputLayout(ID3D11Device*	pd3dDevice, const void* pShaderBytecodeWithInputSignature, size_t BytecodeLength, D3D11_INPUT_ELEMENT_DESC* pLayout, unsigned int layoutSize);

	HRESULT RenderAll(ID3D11DeviceContext* pImmediateContext);

	size_t GetNodeCount(){ return m_meshNodeArray.size(); }

	MESH_NODE& GetNode(const int id){ return m_meshNodeArray[id]; };
	void	GetNodeMatrix(const int id, float* mat4x4){ memcpy(mat4x4, m_meshNodeArray[id].mat4x4, sizeof(float)* 16); };
	MATERIAL_DATA& GetNodeMaterial(const size_t id){ return m_meshNodeArray[id].materialData; };

private:
	FBXLoader*		m_pFBX;

	std::vector<MESH_NODE>	m_meshNodeArray;

	HRESULT CreateNodes(ID3D11Device*	pd3dDevice);
	HRESULT VertexConstruction(ID3D11Device*	pd3dDevice, FBX_MESH_NODE &fbxNode, MESH_NODE& meshNode);
	HRESULT MaterialConstruction(ID3D11Device*	pd3dDevice, FBX_MESH_NODE &fbxNode, MESH_NODE& meshNode);

	HRESULT CreateVertexBuffer(ID3D11Device*	pd3dDevice, ID3D11Buffer** pBuffer, void* pVertices, uint32_t stride, uint32_t vertexCount);
	HRESULT CreateIndexBuffer(ID3D11Device*	pd3dDevice, ID3D11Buffer** pBuffer, void* pIndices, uint32_t indexCount);


};

