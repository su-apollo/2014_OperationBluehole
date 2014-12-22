#include "stdafx.h"
#include "Mesh.h"
#include "Camera.h"
#include "SamplerManager.h"

Mesh::Mesh()
{
	D3DXMatrixIdentity(&mWorld);
}

Mesh::~Mesh()
{
	for (size_t i = 0; i < mMeshNodeArray.size(); ++i)
		mMeshNodeArray[i].Release();
	delete mFBX;
}

void Mesh::RenderNode(ID3D11DeviceContext* dc, MESH_NODE& node)
{
	VS_CONSTBUFF_DATA vcd;
	D3DXMATRIX matWorld = D3DXMATRIX(node.mat4x4) * mWorld;
	D3DXMATRIX matView = Camera::GetInstance()->GetMatView();
	D3DXMATRIX matProj = Camera::GetInstance()->GetMatProj();

	// 열 우선 배치
	D3DXMatrixTranspose(&matWorld, &matWorld);
	D3DXMatrixTranspose(&matView, &matView);
	D3DXMatrixTranspose(&matProj, &matProj);
	vcd.mWorld = matWorld;
	vcd.mView = matView;
	vcd.mProjection = matProj;
	dc->UpdateSubresource(mVSConstBuffer, 0, NULL, &vcd, 0, 0);

	UINT stride = sizeof(MESH_VERTEX);
	UINT offset = 0;
	dc->IASetVertexBuffers(0, 1, &node.m_pVB, &stride, &offset);

	DXGI_FORMAT indexbit = DXGI_FORMAT_R16_UINT;
	if (node.m_indexBit == MESH_NODE::INDEX_32BIT)
		indexbit = DXGI_FORMAT_R32_UINT;
	dc->IASetIndexBuffer(node.m_pIB, indexbit, 0);

	if (node.materialData.pSRVDiffuse)
		dc->PSSetShaderResources(0, 1, &node.materialData.pSRVDiffuse);
	if (node.materialData.pSRVNormal)
		dc->PSSetShaderResources(1, 1, &node.materialData.pSRVNormal);
	if (node.materialData.pSRVSpecular)
		dc->PSSetShaderResources(2, 1, &node.materialData.pSRVSpecular);

	ID3D11SamplerState* linearSampler = SamplerManager::GetInstance()->GetLinearSampler();
	dc->PSSetSamplers(0, 1, &linearSampler);

	dc->DrawIndexed(node.indexCount, 0, 0);
}

BOOL Mesh::CreateVertexBuffer(ID3D11Device* device, ID3D11Buffer** pBuffer, void* pVertices, uint32_t stride, uint32_t vertexCount)
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

	hr = device->CreateBuffer(&bd, &InitData, pBuffer);
	if (FAILED(hr))
		return hr;

	return hr;
}

BOOL Mesh::CreateIndexBuffer(ID3D11Device* device, ID3D11Buffer** pBuffer, void* pIndices, uint32_t indexCount)
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

	hr = device->CreateBuffer(&bd, &InitData, pBuffer);
	if (FAILED(hr))
		return hr;

	return hr;
}

HRESULT Mesh::CompileShaderFromFile(WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut)
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

BOOL Mesh::CreateVSConstBuffer(ID3D11Device* device, UINT constBuffSize)
{
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = constBuffSize;
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;

	hr = device->CreateBuffer(&bd, NULL, &mVSConstBuffer);
	if (FAILED(hr))
		return FALSE;

	return TRUE;
}

BOOL Mesh::CompilePixelShader(ID3D11Device* device, WCHAR* path, LPCSTR main, LPCSTR model)
{
	ID3DBlob* pPSBlob = NULL;
	hr = CompileShaderFromFile(path, main, model, &pPSBlob);
	if (FAILED(hr))
		return FALSE;

	hr = device->CreatePixelShader(pPSBlob->GetBufferPointer(),
		pPSBlob->GetBufferSize(), NULL, &mPixelShader);

	SafeRelease(pPSBlob);
	if (FAILED(hr))
		return FALSE;

	return TRUE;
}

BOOL Mesh::CompileVertexShader(ID3D11Device* device, WCHAR* path, LPCSTR main, LPCSTR model)
{
	ID3DBlob* pVSBlob = NULL;
	hr = CompileShaderFromFile(path, main, model, &pVSBlob);
	if (FAILED(hr))
		return FALSE;

	hr = device->CreateVertexShader(pVSBlob->GetBufferPointer(),
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

	hr = device->CreateInputLayout(layout, numElements, pVSBlob->GetBufferPointer(),
		pVSBlob->GetBufferSize(), &mVertexLayout11);

	SafeRelease(pVSBlob);

	if (FAILED(hr))
		return FALSE;

	return TRUE;
}

BOOL Mesh::CompileShader(ID3D11Device* device, WCHAR* vpath, LPCSTR vmain, LPCSTR vmodel, WCHAR* ppath, LPCSTR pmain, LPCSTR pmodel)
{
	if (!CompileVertexShader(device, vpath, vmain, vmodel))
		return FALSE;

	if (!CompilePixelShader(device, ppath, pmain, pmodel))
		return FALSE;

	return TRUE;
}

BOOL Mesh::VertexConstruction(ID3D11Device* device, FBX_MESH_NODE& fbxNode, MESH_NODE& meshNode)
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

		v = fbxNode.m_tangentArray[i];

		pV[i].vTan = D3DXVECTOR3((float)v.mData[0],
			(float)v.mData[1],
			(float)v.mData[2]);

		// uv에서 v값에 -를 곱해야한다. 이유는 잘 모르겠음
		if ((float)fbxNode.m_texcoordArray.size() > 0)
			/*pV[i].vTexcoord = D3DXVECTOR2((float)abs(1.0f - fbxNode.m_texcoordArray[i].mData[0]),
			(float)abs(1.0f - fbxNode.m_texcoordArray[i].mData[1]));*/
			pV[i].vTexcoord = D3DXVECTOR2(fbxNode.m_texcoordArray[i].mData[0], -1.0f * fbxNode.m_texcoordArray[i].mData[1]);
		else
			pV[i].vTexcoord = D3DXVECTOR2(0, 0);
	}

	// todo : create tangent
	//

	CreateVertexBuffer(device, &meshNode.m_pVB, pV, sizeof(VERTEX_DATA), meshNode.vertexCount);

	if (pV)
		delete[] pV;

	return hr;
}

BOOL Mesh::CreateNode(ID3D11Device* device)
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

		VertexConstruction(device, fbxNode, meshNode);

		meshNode.indexCount = static_cast<DWORD>(fbxNode.indexArray.size());
		meshNode.SetIndexBit(meshNode.indexCount);

		if (fbxNode.indexArray.size() > 0)
			CreateIndexBuffer(device, &meshNode.m_pIB, &fbxNode.indexArray[0], static_cast<uint32_t>(fbxNode.indexArray.size()));

		memcpy(meshNode.mat4x4, fbxNode.mat4x4, sizeof(float) * 16);

		mMeshNodeArray.push_back(meshNode);
	}

	MaterialConstruction(device);

	return TRUE;
}

BOOL Mesh::LoadFBX(ID3D11Device* device, const char* path)
{
	mFBX = new CFBXLoader;

	hr = mFBX->LoadFBX(path, CFBXLoader::eAXIS_OPENGL);
	if (FAILED(hr))
		return FALSE;

	if (!CreateNode(device))
		return FALSE;

	return TRUE;
}

void Mesh::RenderAll(ID3D11DeviceContext* dc)
{
	dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	dc->VSSetShader(mVertexShader, NULL, 0);
	dc->VSSetConstantBuffers(0, 1, &mVSConstBuffer);
	dc->PSSetShader(mPixelShader, NULL, 0);
	dc->PSSetConstantBuffers(0, 1, &mPSConstBuffer);

	dc->IASetInputLayout(mVertexLayout11);

	size_t nodeCount = mMeshNodeArray.size();

	if (nodeCount == 0)
		return;

	for (size_t i = 0; i < nodeCount; ++i)
		RenderNode(dc, mMeshNodeArray[i]);

	CleanUp(dc);
}

void Mesh::CleanUp(ID3D11DeviceContext* dc)
{
	ID3D11ShaderResourceView* nullSRV[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
	dc->VSSetShaderResources(0, 8, nullSRV);
	dc->PSSetShaderResources(0, 8, nullSRV);
	dc->VSSetShader(0, 0, 0);
	dc->PSSetShader(0, 0, 0);
}


