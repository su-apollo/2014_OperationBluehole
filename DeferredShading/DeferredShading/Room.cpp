#include "stdafx.h"
#include "Room.h"
#include "Renderer.h"
#include "Timer.h"
#include "SamplerManager.h"

Room::Room()
{
}


Room::~Room()
{
}

BOOL Room::Init()
{
	mD3DDevice = Renderer::GetInstance()->GetDevice();
	mD3DDeviceContext = Renderer::GetInstance()->GetDeviceContext();
	D3DXMatrixIdentity(&mWorld);

	mFbxScene = ModelManager::GetInstance()->MakeFbxSceneFromFile(ROOM_PATH);

	if (mFbxScene == NULL)
		return FALSE;

	GetMeshData(mFbxScene->GetRootNode());

	if (!CompileShader())
		return FALSE;

	CreateModelBuffer();
	LoadTexture();

	return TRUE;
}


void Room::GetMeshData(FbxNode* inNode)
{
	if (inNode)
	{
		int ChildCount = inNode->GetChildCount();

		for (int i = 0; i < ChildCount; i++)
		{
			FbxNode* childNode = inNode->GetChild(i);

			if (childNode->GetNodeAttribute() == NULL)
				continue;

			int temp = childNode->GetNodeAttribute()->GetAttributeType();

			if (childNode->GetNodeAttribute()->GetAttributeType() != FbxNodeAttribute::eMesh)
				continue;

			mModel.push_back(ModelManager::GetInstance()->ProcessMesh(childNode));
			GetMeshData(childNode);
		}
	}
}




void Room::CleanUp()
{
	ModelManager::GetInstance()->CleanUp();
}

BOOL Room::CompileShader()
{
	if (!CompileVertexShader())
		return FALSE;

	if (!CompilePixelShader())
		return FALSE;

	return TRUE;
}


void Room::Render()
{
	// rotate
	D3DXMATRIX matRotate;
	D3DXMatrixRotationY(&matRotate, Timer::GetInstance()->GetDeltaTime());
	//mWorld *= matRotate;

	mD3DDeviceContext->VSSetShader(mVertexShader, NULL, 0);
	mD3DDeviceContext->VSSetConstantBuffers(0, 1, &mVSConstBuffer);
	mD3DDeviceContext->PSSetShader(mPixelShader, NULL, 0);
	mD3DDeviceContext->PSSetConstantBuffers(0, 1, &mPSConstBuffer);

	for (unsigned int i = 0; i < mMeshData.size(); ++i)
		RenderMesh(mMeshData[i]);
}

void Room::RenderMesh(MeshDataPointer meshData)
{
	// update constbuff
	D3DXMATRIX matWorld = meshData->mWorld* mWorld;
	D3DXMATRIX matView = Camera::GetInstance()->GetMatView();
	D3DXMATRIX matProj = Camera::GetInstance()->GetMatProj();
	VSConstantBuffer vcb;

	// 열 우선배치
	D3DXMatrixTranspose(&matWorld, &matWorld);
	D3DXMatrixTranspose(&matView, &matView);
	D3DXMatrixTranspose(&matProj, &matProj);
	vcb.mWorld = matWorld;
	vcb.mView = matView;
	vcb.mProjection = matProj;
	mD3DDeviceContext->UpdateSubresource(mVSConstBuffer, 0, NULL, &vcb, 0, 0);

	mD3DDeviceContext->IASetInputLayout(mVertexLayout11);

	// Set vertex buffer
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	mD3DDeviceContext->IASetVertexBuffers(0, 1, &meshData->mVertexBuffer, &stride, &offset);

	// Set index buffer
	mD3DDeviceContext->IASetIndexBuffer(meshData->mIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// Set primitive topology
	mD3DDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	mD3DDeviceContext->PSSetShaderResources(0, 1, &meshData->mTextureRVDiff);
	mD3DDeviceContext->PSSetShaderResources(1, 1, &meshData->mTextureRVNorm);
	mD3DDeviceContext->PSSetShaderResources(2, 1, &meshData->mTextureRVSpec);

	ID3D11SamplerState* linearSampler = SamplerManager::GetInstance()->GetLinearSampler();
	mD3DDeviceContext->PSSetSamplers(0, 1, &linearSampler);

	mD3DDeviceContext->DrawIndexed(meshData->mNumIndex, 0, 0);
}

HRESULT Room::CompileShaderFromFile(WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut)
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

BOOL Room::CompileVertexShader()
{
	ID3DBlob* pVSBlob = NULL;
	hr = CompileShaderFromFile(const_cast<WCHAR*>(ROOM_VS_PATH), ROOM_VS_MAIN, ROOM_VS_MODEL, &pVSBlob);
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

BOOL Room::CompilePixelShader()
{
	ID3DBlob* pPSBlob = NULL;
	hr = CompileShaderFromFile(const_cast<WCHAR*>(ROOM_PS_PATH), ROOM_PS_MAIN, ROOM_PS_MODEL, &pPSBlob);
	if (FAILED(hr))
		return FALSE;

	hr = mD3DDevice->CreatePixelShader(pPSBlob->GetBufferPointer(),
		pPSBlob->GetBufferSize(), NULL, &mPixelShader);

	SafeRelease(pPSBlob);
	if (FAILED(hr))
		return FALSE;

	return TRUE;
}



void Room::Release()
{
	ModelManager::Release();
	SafeRelease(mVSConstBuffer);
	SafeRelease(mPSConstBuffer);
	SafeRelease(mVertexLayout11);
	SafeRelease(mVertexShader);
	SafeRelease(mPixelShader);
}


BOOL Room::CreateModelBuffer()
{
	for (unsigned int i = 0; i < mModel.size(); ++i)
	{
		if (!CreateMeshBuffer(mModel[i]))
			return FALSE;
	}

	return TRUE;
}


BOOL Room::CreateMeshBuffer(MeshPointer mesh)
{
	MeshDataPointer pMeshData(new MeshData);
	mMeshData.push_back(pMeshData);
	pMeshData->mWorld = mesh->mWorld;

	if (!CreateMeshVB(mesh, mMeshData.back()))
		return FALSE;
	if (!CreateMeshIB(mesh, mMeshData.back()))
		return FALSE;
	if (!CreateMeshCB(mesh, mMeshData.back()))
		return FALSE;

	return TRUE;
}

BOOL Room::CreateMeshVB(MeshPointer mesh, MeshDataPointer meshData)
{
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));

	// Create vertex buffer
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(Vertex)* mesh->mNumVertex;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;
	InitData.pSysMem = &(mesh->mVertex.front());
	hr = mD3DDevice->CreateBuffer(&bd, &InitData, &meshData->mVertexBuffer);
	if (FAILED(hr))
		return FALSE;

	return TRUE;
}


BOOL Room::CreateMeshIB(MeshPointer mesh, MeshDataPointer meshData)
{
	meshData->mNumIndex = mesh->mNumIndex;

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));

	// Create index buffer
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(unsigned int)* mesh->mNumIndex;
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;
	InitData.pSysMem = &(mesh->mIndices.front());
	hr = mD3DDevice->CreateBuffer(&bd, &InitData, &meshData->mIndexBuffer);
	if (FAILED(hr))
		return FALSE;

	return TRUE;

}


BOOL Room::CreateMeshCB(MeshPointer mesh, MeshDataPointer meshData)
{
	D3D11_BUFFER_DESC bd;
	//아래 zeromemory를 꼭해야함
	ZeroMemory(&bd, sizeof(bd));
	// Create the constant buffer
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(VSConstantBuffer);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;

	hr = mD3DDevice->CreateBuffer(&bd, NULL, &mVSConstBuffer);
	if (FAILED(hr))
		return FALSE;

	return TRUE;

}

BOOL Room::LoadTexture()
{
	mModel[0]->mTexutreDiff = ROOM_TEXTURE_DIFF;
	mModel[0]->mTexutreNorm = ROOM_TEXTURE_NORM;

	// Load the Texture
	hr = D3DX11CreateShaderResourceViewFromFile(mD3DDevice, mModel[0]->mTexutreDiff, NULL, NULL, &mMeshData[0]->mTextureRVDiff, NULL);
	if (FAILED(hr))
		return FALSE;

	hr = D3DX11CreateShaderResourceViewFromFile(mD3DDevice, mModel[0]->mTexutreNorm, NULL, NULL, &mMeshData[0]->mTextureRVNorm, NULL);
	if (FAILED(hr))
		return FALSE;

	return TRUE;
}

BOOL Room::LoadMeshTexture(MeshPointer mesh, MeshDataPointer meshData)
{
	return TRUE;
}


