#include "stdafx.h"
#include "TestMesh.h"
#include "Renderer.h"
#include "SamplerManager.h"


TestMesh::TestMesh()
{
}


TestMesh::~TestMesh()
{
}

BOOL TestMesh::Init()
{
	mD3DDevice = Renderer::GetInstance()->GetDevice();
	mD3DDeviceContext = Renderer::GetInstance()->GetDeviceContext();

	if (!LoadFBX())
		return FALSE;

	if (!CompileShader())
		return FALSE;

	if (!CreateVSConstBuffer())
		return FALSE;

	return TRUE;
}

BOOL TestMesh::LoadFBX()
{
	mFBX = new CFBXLoader;

	hr = mFBX->LoadFBX(ELIN_PATH, CFBXLoader::eAXIS_OPENGL);
	if (FAILED(hr))
		return FALSE;

	if (!CreateNode())
		return FALSE;

	return TRUE;
}

BOOL TestMesh::CreateNode()
{
	size_t nodeCoount = mFBX->GetNodesCount();
	if (nodeCoount == 0)
		return FALSE;

	for (size_t i = 0; i < nodeCoount; i++)
	{
		MESH_NODE meshNode;
		FBX_MESH_NODE fbxNode = mFBX->GetNode(static_cast<unsigned int>(i));

		// 루트 노드는 제외하고 생성
		if (fbxNode.name == "RootNode")
			continue;

		VertexConstruction(fbxNode, meshNode);

		meshNode.indexCount = static_cast<DWORD>(fbxNode.indexArray.size());
		meshNode.SetIndexBit(meshNode.indexCount);

		if (fbxNode.indexArray.size() > 0)
			CreateIndexBuffer(&meshNode.m_pIB, &fbxNode.indexArray[0], static_cast<uint32_t>(fbxNode.indexArray.size()));

		memcpy(meshNode.mat4x4, fbxNode.mat4x4, sizeof(float) * 16);

		mMeshNodeArray.push_back(meshNode);
	}

	MaterialConstruction();

	return TRUE;
}

BOOL TestMesh::VertexConstruction(FBX_MESH_NODE& fbxNode, MESH_NODE& meshNode)
{
	meshNode.vertexCount = static_cast<DWORD>(fbxNode.m_positionArray.size());
	if (meshNode.vertexCount == 0)
		return FALSE;

	VERTEX_DATA* pV = new VERTEX_DATA[meshNode.vertexCount];

	for (size_t i = 0; i < meshNode.vertexCount; i++)
	{
		FbxVector4 v = fbxNode.m_positionArray[i];
		pV[i].vPos = D3DXVECTOR3((float)v.mData[0],
			(float)v.mData[1],
			(float)v.mData[2]);

		v = fbxNode.m_normalArray[i];
		
		pV[i].vNor = D3DXVECTOR3((float)v.mData[0],
			(float)v.mData[1],
			(float)v.mData[2]);

		if ((float)fbxNode.m_texcoordArray.size() > 0)
			pV[i].vTexcoord = D3DXVECTOR2((float)abs(1.0f - fbxNode.m_texcoordArray[i].mData[0]),
				(float)abs(1.0f - fbxNode.m_texcoordArray[i].mData[1]));
		else
			pV[i].vTexcoord = D3DXVECTOR2(0, 0);
	}

	// todo : create tangent
	//

	CreateVertexBuffer(&meshNode.m_pVB, pV, sizeof(VERTEX_DATA), meshNode.vertexCount);

	if (pV)
		delete[] pV;
	
	return hr;
}

BOOL TestMesh::MaterialConstruction()
{
	mMeshNodeArray[0].materialData.diffuseTexPath = ELIN_TEXTURE_HAND_DIFF;
	mMeshNodeArray[0].materialData.specularTexPath = ELIN_TEXTURE_HAND_SPEC;
	mMeshNodeArray[0].materialData.normalTexPath = ELIN_TEXTURE_HAND_NORM;

	mMeshNodeArray[1].materialData.diffuseTexPath = ELIN_TEXTURE_LEG_DIFF;
	mMeshNodeArray[1].materialData.specularTexPath = ELIN_TEXTURE_LEG_SPEC;
	mMeshNodeArray[1].materialData.normalTexPath = ELIN_TEXTURE_LEG_NORM;

	mMeshNodeArray[2].materialData.diffuseTexPath = ELIN_TEXTURE_BODY_DIFF;
	mMeshNodeArray[2].materialData.specularTexPath = ELIN_TEXTURE_BODY_SPEC;
	mMeshNodeArray[2].materialData.normalTexPath = ELIN_TEXTURE_BODY_NORM;

	mMeshNodeArray[3].materialData.diffuseTexPath = ELIN_TEXTURE_HAIR_DIFF;
	mMeshNodeArray[3].materialData.specularTexPath = ELIN_TEXTURE_HAIR_SPEC;
	mMeshNodeArray[3].materialData.normalTexPath = ELIN_TEXTURE_HAIR_NORM;

	mMeshNodeArray[4].materialData.diffuseTexPath = ELIN_TEXTURE_FACE_DIFF;
	mMeshNodeArray[4].materialData.specularTexPath = ELIN_TEXTURE_FACE_SPEC;
	mMeshNodeArray[4].materialData.normalTexPath = ELIN_TEXTURE_FACE_NORM;

	for (size_t i = 0; i < mMeshNodeArray.size(); ++i)
	{
		BOOL result = CreateMeshTexture(mMeshNodeArray[i]);
		if (!result)
			return FALSE;
	}

	return TRUE;
}

BOOL TestMesh::CreateMeshTexture(MESH_NODE& meshNode)
{
	hr = D3DX11CreateShaderResourceViewFromFile(mD3DDevice, meshNode.materialData.diffuseTexPath, NULL, NULL, &meshNode.materialData.pSRVDiffuse, NULL);
	if (FAILED(hr))
		return FALSE;
	hr = D3DX11CreateShaderResourceViewFromFile(mD3DDevice, meshNode.materialData.specularTexPath, NULL, NULL, &meshNode.materialData.pSRVSpecular, NULL);
	if (FAILED(hr))
		return FALSE;
	hr = D3DX11CreateShaderResourceViewFromFile(mD3DDevice, meshNode.materialData.normalTexPath, NULL, NULL, &meshNode.materialData.pSRVNormal, NULL);
	if (FAILED(hr))
		return FALSE;

	return TRUE;
}

BOOL TestMesh::CreateVertexBuffer(ID3D11Buffer** pBuffer, void* pVertices, uint32_t stride, uint32_t vertexCount)
{
	if (stride == 0 || vertexCount == 0)
		return E_FAIL;

	HRESULT hr = S_OK;

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = stride * vertexCount;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;
	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));

	InitData.pSysMem = pVertices;

	hr = mD3DDevice->CreateBuffer(&bd, &InitData, pBuffer);
	if (FAILED(hr))
		return hr;

	return hr;
}

BOOL TestMesh::CreateIndexBuffer(ID3D11Buffer** pBuffer, void* pIndices, uint32_t indexCount)
{
	if (indexCount == 0)
		return E_FAIL;

	HRESULT hr = S_OK;
	size_t stride = sizeof(unsigned int);

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = static_cast<uint32_t>(stride*indexCount);
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;
	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));

	InitData.pSysMem = pIndices;

	hr = mD3DDevice->CreateBuffer(&bd, &InitData, pBuffer);
	if (FAILED(hr))
		return hr;

	return hr;
}

BOOL TestMesh::CompileShader()
{
	if (!CompileVertexShader())
		return FALSE;

	if (!CompilePixelShader())
		return FALSE;

	return TRUE;
}

BOOL TestMesh::CompileVertexShader()
{
	ID3DBlob* pVSBlob = NULL;
	hr = CompileShaderFromFile(const_cast<WCHAR*>(ELIN_VS_PATH), ELIN_VS_MAIN, ELIN_VS_MODEL, &pVSBlob);
	if (FAILED(hr))
		return FALSE;

	hr = mD3DDevice->CreateVertexShader(pVSBlob->GetBufferPointer(),
		pVSBlob->GetBufferSize(), NULL, &mVertexShader);

	if (FAILED(hr))
	{
		SafeRelease(pVSBlob);
		return FALSE;
	}

	// input layout
	const D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	UINT numElements = ARRAYSIZE(layout);

	hr = mD3DDevice->CreateInputLayout(layout, numElements, pVSBlob->GetBufferPointer(),
		pVSBlob->GetBufferSize(), &mVertexLayout11);

	SafeRelease(pVSBlob);

	if (FAILED(hr))
		return FALSE;

	return TRUE;
}

BOOL TestMesh::CompilePixelShader()
{
	ID3DBlob* pPSBlob = NULL;
	hr = CompileShaderFromFile(const_cast<WCHAR*>(ELIN_PS_PATH), ELIN_PS_MAIN, ELIN_PS_MODEL, &pPSBlob);
	if (FAILED(hr))
		return FALSE;

	hr = mD3DDevice->CreatePixelShader(pPSBlob->GetBufferPointer(),
		pPSBlob->GetBufferSize(), NULL, &mPixelShader);

	SafeRelease(pPSBlob);
	if (FAILED(hr))
		return FALSE;

	return TRUE;
}

HRESULT TestMesh::CompileShaderFromFile(WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut)
{
	DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_PACK_MATRIX_COLUMN_MAJOR;

#if defined( DEBUG ) || defined( _DEBUG )
	dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif

	ID3DBlob* pErrorBlob;
	hr = D3DX11CompileFromFile(szFileName, NULL, NULL, szEntryPoint, szShaderModel,
		dwShaderFlags, 0, NULL, ppBlobOut, &pErrorBlob, NULL);
	if (FAILED(hr))
	{
		if (pErrorBlob != NULL)
			OutputDebugStringA((char*)pErrorBlob->GetBufferPointer());
		if (pErrorBlob) pErrorBlob->Release();
		return hr;
	}
	if (pErrorBlob) pErrorBlob->Release();

	return S_OK;
}

void TestMesh::RenderAll()
{
	mD3DDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	mD3DDeviceContext->VSSetShader(mVertexShader, NULL, 0);
	mD3DDeviceContext->VSSetConstantBuffers(0, 1, &mVSConstBuffer);
	mD3DDeviceContext->PSSetShader(mPixelShader, NULL, 0);
	mD3DDeviceContext->PSSetConstantBuffers(0, 1, &mPSConstBuffer);

	mD3DDeviceContext->IASetInputLayout(mVertexLayout11);

	size_t nodeCount = mMeshNodeArray.size();

	if (nodeCount == 0)
		return;

	for (size_t i = 0; i < nodeCount; ++i)
		RenderNode(mMeshNodeArray[i]);
}

void TestMesh::RenderNode(MESH_NODE& node)
{
	VS_CONSTBUFF_DATA vcd;
	D3DXMATRIX matWorld = node.mat4x4;
	D3DXMATRIX matView = Camera::GetInstance()->GetMatView();
	D3DXMATRIX matProj = Camera::GetInstance()->GetMatProj();

	// 열 우선 배치
	D3DXMatrixTranspose(&matWorld, &matWorld);
	D3DXMatrixTranspose(&matView, &matView);
	D3DXMatrixTranspose(&matProj, &matProj);
	vcd.mWorld = matWorld;
	vcd.mView = matView;
	vcd.mProjection = matProj;
	mD3DDeviceContext->UpdateSubresource(mVSConstBuffer, 0, NULL, &vcd, 0, 0);

	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	mD3DDeviceContext->IASetVertexBuffers(0, 1, &node.m_pVB, &stride, &offset);

	DXGI_FORMAT indexbit = DXGI_FORMAT_R16_UINT;
	if (node.m_indexBit == MESH_NODE::INDEX_32BIT)
		indexbit = DXGI_FORMAT_R32_UINT;
	mD3DDeviceContext->IASetIndexBuffer(node.m_pIB, indexbit, 0);

	mD3DDeviceContext->PSSetShaderResources(0, 1, &node.materialData.pSRVDiffuse);
	mD3DDeviceContext->PSSetShaderResources(1, 1, &node.materialData.pSRVNormal);
	mD3DDeviceContext->PSSetShaderResources(2, 1, &node.materialData.pSRVSpecular);

	ID3D11SamplerState* linearSampler = SamplerManager::GetInstance()->GetLinearSampler();
	mD3DDeviceContext->PSSetSamplers(0, 1, &linearSampler);

	mD3DDeviceContext->DrawIndexed(node.indexCount, 0, 0);
}

BOOL TestMesh::CreateVSConstBuffer()
{
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(VS_CONSTBUFF_DATA);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;

	hr = mD3DDevice->CreateBuffer(&bd, NULL, &mVSConstBuffer);
	if (FAILED(hr))
		return FALSE;

	return TRUE;
}


